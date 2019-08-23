/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/brave_site_hacks_network_delegate_helper.h"

#include <memory>
#include <string>
#include <vector>

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

const std::vector<const std::string>& GetQueryStringTrackers() {
  static const base::NoDestructor<std::vector<const std::string>> trackers(
      {"fbclid", "gclid", "msclkid", "mc_eid"});
  return *trackers;
}

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

void ApplyPotentialQueryStringFilter(const GURL& request_url,
                                     std::string* new_url_spec) {
  std::string new_query = request_url.query();

  bool modified = false;
  for (auto tracker : GetQueryStringTrackers()) {
    size_t key_size = 0;
    size_t key_start = std::string::npos;

    // Look for the tracker anywhere in the query string
    // (e.g. ?...fbclid=1234...).
    {
      const std::string key = tracker + "=";
      key_size = key.size();
      key_start = new_query.find(key, 0);
    }
    if (key_start == std::string::npos) {
      continue;
    }

    // Check to see if the tracker is preceded by a '&'
    // (e.g. ?...&fbclid=1234...).
    if (key_start != 0) {
      if (new_query[--key_start] != '&') {
        // Tracker is substring of another key (e.g. ?...&abc-fbclid=1234...).
        continue;
      }
      key_size++;
    }

    {
      // Find the size of the tracking parameter value.
      const size_t value_start = key_start + key_size;
      size_t value_end = new_query.find("&", key_start + 1);
      if (value_end == std::string::npos) {
        value_end = new_query.size();  // Tracker is the last query parameter.
      }
      if (value_end <= value_start) {
        DCHECK_EQ(value_end, value_start);
        continue;  // Empty value, no need to remove tracker.
      }

      // Remove tracking parameter and its value from the string.
      modified = true;
      new_query.erase(key_start, value_end - key_start);
    }

    // Remove the trailing '&' if we removed the first parameter
    // (e.g. ?fbclid=1234&...).
    if (!new_query.empty() && key_start == 0) {
      DCHECK_EQ(new_query[0], '&');
      new_query.erase(key_start, 1);
    }

    if (new_query.empty()) {
      break;  // Nothing left to filter.
    }
  }

  if (modified) {
    url::Replacements<char> replacements;
    if (new_query.empty()) {
      replacements.ClearQuery();
    } else {
      replacements.SetQuery(new_query.c_str(),
                            url::Component(0, new_query.size()));
    }
    *new_url_spec = request_url.ReplaceComponents(replacements).spec();
  }
}

}  // namespace

int OnBeforeURLRequest_SiteHacksWork(
    const ResponseCallback& next_callback,
    std::shared_ptr<BraveRequestInfo> ctx) {
  ApplyPotentialReferrerBlock(ctx);

  if (ctx->request_url.has_query()) {
    ApplyPotentialQueryStringFilter(ctx->request_url, &ctx->new_url_spec);
  }
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
