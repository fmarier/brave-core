/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "net/http/http_server_properties_impl.h"

namespace {

// Override the expiry of cross-origin alternative services.
base::Time BraveAlternativeServiceExpiration(
    const url::SchemeHostPort& origin,
    const net::AlternativeService& alternative_service,
    base::Time now, base::Time expiration) {

  if (origin.host() != alternative_service.host ||
      origin.port() != alternative_service.port) {
    expiration = std::max(expiration, now);  // Ignore dates in the past.
    expiration = std::min(expiration, now + base::TimeDelta::FromSeconds(10));
  }
  return expiration;
}

}  // namespace

#define BRAVE_RESTRICT_ALTERNATIVE_SERVICES_EXPIRATION \
  expiration = BraveAlternativeServiceExpiration(origin, alternative_service,\
                                                 clock_->Now(), expiration);

#include "../../../../net/http/http_server_properties_impl.cc"
#undef BRAVE_RESTRICT_ALTERNATIVE_SERVICES_EXPIRATION
