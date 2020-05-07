/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/common/brave_content_client.h"

#include "base/big_endian.h"
#include "content/common/url_schemes.h"
#include "content/public/common/url_constants.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

TEST(BraveContentClientTest, AdditionalSchemesTest) {
  BraveContentClient content_client;
  content::SetContentClient(&content_client);
  content::ReRegisterContentSchemesForTests();

  const GURL sync_url("brave://sync");
  EXPECT_TRUE(sync_url.is_valid());
  EXPECT_TRUE(sync_url.has_host());
  EXPECT_EQ("sync", sync_url.host());

  const GURL chrome_sync_url("chrome://sync");
  EXPECT_TRUE(chrome_sync_url.is_valid());
  EXPECT_TRUE(chrome_sync_url.has_host());
  EXPECT_EQ("sync", chrome_sync_url.host());
}

 bool RemovePadding(std::string& padded_string) {
  if (padded_string.size() < 4) {
    return false;  // Missing length field
  }

  // Read payload length from the header.
  uint32_t data_length;
  base::ReadBigEndian(&padded_string[0], &data_length);

  // Remove length header.
  padded_string.erase(0, 4);
  if (padded_string.size() < data_length) {
    return false;  // Payload shorter than expected length
  }

  // Remove padding.
  padded_string.resize(data_length);
  return true;
}

TEST(BraveContentClientTest, RemovePadding) {
  // TODO: turn this into an array of character arrays
  const char test_case[] = {
      0, 0, 0, 4, 'A', 'B', 'C', 'D', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
      'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
      'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'
  };
  std::string normal1(test_case, sizeof(test_case));

  EXPECT_TRUE(RemovePadding(normal1));
  EXPECT_EQ(normal1, "ABCD");
}
