/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/brave_site_hacks_network_delegate_helper.h"

#include <memory>
#include <string>

#include "base/sequenced_task_runner.h"
#include "base/strings/string_util.h"
#include "brave/common/network_constants.h"
#include "brave/common/shield_exceptions.h"
#include "brave/common/url_constants.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "brave/components/brave_shields/browser/brave_shields_web_contents_observer.h"
#include "brave/components/brave_shields/common/brave_shield_constants.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/common/referrer.h"
#include "extensions/common/url_pattern.h"
#include "net/url_request/url_request.h"

using content::BrowserThread;
using content::Referrer;

namespace brave {

namespace {

bool ApplyPotentialReferrerBlock(std::shared_ptr<BraveRequestInfo> ctx) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  GURL target_origin = ctx->request_url.GetOrigin();
  GURL tab_origin = ctx->tab_origin;
  if (tab_origin.SchemeIs(kChromeExtensionScheme)) {
    return false;
  }

  const std::string original_referrer = ctx->referrer.spec();
  Referrer new_referrer;
  if (brave_shields::ShouldSetReferrer(
          ctx->allow_referrers, ctx->allow_brave_shields,
          GURL(original_referrer), tab_origin, ctx->request_url, target_origin,
          Referrer::NetReferrerPolicyToBlinkReferrerPolicy(
              ctx->referrer_policy), &new_referrer)) {
    ctx->new_referrer = new_referrer.url;
    return true;
  }
  return false;
}

bool ApplyPotentialQueryStringFilter(std::shared_ptr<BraveRequestInfo> ctx) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  if (ctx->request_url.has_query()) {
    bool modified = false;
    std::vector<std::string> new_query_parts;

    const std::string query = ctx->request_url.query();
    url::Component cursor(0, query.size());
    url::Component key_range, value_range;
    while (url::ExtractQueryKeyValue(query.data(), &cursor, &key_range,
                                     &value_range)) {
      const base::StringPiece key(query.data() + key_range.begin,
                                  key_range.len);
      const base::StringPiece value(query.data() + value_range.begin,
                                    value_range.len);
      if (!value.empty() && (key == "fbclid" || key == "gclid" ||
                             key == "msclkid" || key == "mc_eid")) {
        modified = true;  // We'll have to rewrite the query string.
      } else if (!key.empty() || !value.empty()) {
        // Add the current key=value to the new query string.
        new_query_parts.push_back(base::StringPrintf("%s=%s",
            key.as_string().c_str(), value.as_string().c_str()));
      }
    }

    if (modified) {
      url::Replacements<char> replacements;
      if (new_query_parts.empty()) {
        replacements.ClearQuery();
      } else {
        std::string new_query_string = base::JoinString(new_query_parts, "&");
        url::Component new_query(0, new_query_string.size());
        replacements.SetQuery(new_query_string.c_str(), new_query);
      }
      ctx->new_url_spec =
          ctx->request_url.ReplaceComponents(replacements).spec();
      return true;
    }
  }

  return false;
}

}  // namespace

int OnBeforeURLRequest_SiteHacksWork(
    const ResponseCallback& next_callback,
    std::shared_ptr<BraveRequestInfo> ctx) {
  ApplyPotentialReferrerBlock(ctx);
  ApplyPotentialQueryStringFilter(ctx);
  return net::OK;
}

void CheckForCookieOverride(const GURL& url, const URLPattern& pattern,
    net::HttpRequestHeaders* headers, const std::string& extra_cookies) {
  if (pattern.MatchesURL(url)) {
    std::string cookies;
    if (headers->GetHeader(kCookieHeader, &cookies)) {
      cookies = "; ";
    }
    cookies += extra_cookies;
    headers->SetHeader(kCookieHeader, cookies);
  }
}

int OnBeforeStartTransaction_SiteHacksWork(
    net::HttpRequestHeaders* headers,
    const ResponseCallback& next_callback,
    std::shared_ptr<BraveRequestInfo> ctx) {
  // TODO(bridiver): Fix the Forbes cookie override with enabled NetworkService.
  CheckForCookieOverride(ctx->request_url,
      URLPattern(URLPattern::SCHEME_ALL, kForbesPattern), headers,
      kForbesExtraCookies);
  if (IsUAWhitelisted(ctx->request_url)) {
    std::string user_agent;
    if (headers->GetHeader(kUserAgentHeader, &user_agent)) {
      base::ReplaceFirstSubstringAfterOffset(&user_agent, 0,
        "Chrome", "Brave Chrome");
      headers->SetHeader(kUserAgentHeader, user_agent);
      ctx->set_headers.insert(kUserAgentHeader);
    }
  }
  return net::OK;
}

}  // namespace brave
