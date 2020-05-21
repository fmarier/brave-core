/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

//#include "base/bind.h"
#include "base/path_service.h"
#include "base/run_loop.h"
//#include "base/stl_util.h"
#include "brave/common/brave_paths.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "net/dns/mock_host_resolver.h"
//#include "net/http/http_request_headers.h"
#include "net/test/embedded_test_server/embedded_test_server.h"
#include "net/test/embedded_test_server/http_request.h"
//#include "third_party/blink/public/common/client_hints/client_hints.h"

//#include "net/cookies/cookie_constants.h"
//#include "net/cookies/cookie_options.h"
//#include "testing/gtest/include/gtest/gtest.h"
//#include "url/gurl.h"

const char kCookiePage1[] = "/cookie-expiry1.html";

class BraveCanonicalCookieBrowserTest : public InProcessBrowserTest {
 public:
  BraveCanonicalCookieBrowserTest()
      : https_server_(net::EmbeddedTestServer::TYPE_HTTPS) {
    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);

    https_server_.ServeFilesFromDirectory(test_data_dir);
    // https_server_.RegisterRequestMonitor(
    //     base::BindRepeating(&ClientHintsBrowserTest::MonitorResourceRequest,
    //                         base::Unretained(this)));

    EXPECT_TRUE(https_server_.Start());

    cookie_page1_url_ = https_server_.GetURL(kCookiePage1);
  }

  ~BraveCanonicalCookieBrowserTest() override {}

  void SetUp() override { InProcessBrowserTest::SetUp(); }

  void SetUpOnMainThread() override {
    host_resolver()->AddRule("*", "127.0.0.1");
    base::RunLoop().RunUntilIdle();
  }

  void TearDownOnMainThread() override {}

  const GURL& cookie_page1_url() const { return cookie_page1_url_; }

 private:
  net::EmbeddedTestServer https_server_;
  GURL cookie_page1_url_;

  DISALLOW_COPY_AND_ASSIGN(BraveCanonicalCookieBrowserTest);
};

TEST(BraveCanonicalCookieBrowserTest, FarFutureExpires) {
  ui_test_utils::NavigateToURL(browser(), cookie_page1_url());
  EXPECT_TRUE(true);  // TODO: remove this useless check
}
