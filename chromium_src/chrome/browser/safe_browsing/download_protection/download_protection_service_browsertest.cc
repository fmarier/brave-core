/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "base/path_service.h"
#include "base/strings/stringprintf.h"
#include "brave/common/brave_paths.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "components/safe_browsing/content/web_ui/safe_browsing_ui.h"
#include "components/safe_browsing/core/proto/csd.pb.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_manager.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/download_test_observer.h"
#include "content/public/test/test_utils.h"
#include "net/dns/mock_host_resolver.h"
#include "net/test/embedded_test_server/default_handlers.h"
#include "net/test/embedded_test_server/http_request.h"
#include "ui/base/window_open_disposition.h"

class BraveDownloadProtectionServiceBrowserTest : public InProcessBrowserTest {
 public:
  BraveDownloadProtectionServiceBrowserTest()
      : https_server_(net::EmbeddedTestServer::TYPE_HTTPS) {}

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    host_resolver()->AddRule("*", "127.0.0.1");

    brave::RegisterPathProvider();
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir_);
    https_server_.ServeFilesFromDirectory(test_data_dir_);
    https_server_.AddDefaultHandlers(GetChromeTestDataDir());
    safe_browsing::WebUIInfoSingleton::GetInstance()->AddListenerForTesting();

    ASSERT_TRUE(https_server_.Start());

    download_url_ = https_server_.GetURL("a.com", "/test.exe");
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    InProcessBrowserTest::SetUpCommandLine(command_line);
    // This is needed to load pages from "domain.com" without an interstitial.
    command_line->AppendSwitch(switches::kIgnoreCertificateErrors);
  }

  void DownloadAndWait(GURL url) {
    content::DownloadManager* download_manager =
        content::BrowserContext::GetDownloadManager(browser()->profile());
    content::DownloadTestObserverTerminal observer(
        download_manager, 1,
        content::DownloadTestObserver::ON_DANGEROUS_DOWNLOAD_IGNORE);

    // This call will block until the navigation completes, but will not wait
    // for the download to finish.
    ui_test_utils::NavigateToURLWithDisposition(
        browser(), url, WindowOpenDisposition::CURRENT_TAB,
        ui_test_utils::BROWSER_TEST_WAIT_FOR_LOAD_STOP);

    observer.WaitForFinished();
  }

  const net::EmbeddedTestServer& https_server() { return https_server_; }

  const GURL& download_url() { return download_url_; }

  content::WebContents* contents() {
    return browser()->tab_strip_model()->GetActiveWebContents();
  }

 private:
  GURL download_url_;
  base::FilePath test_data_dir_;

  net::test_server::EmbeddedTestServer https_server_;
};

IN_PROC_BROWSER_TEST_F(BraveDownloadProtectionServiceBrowserTest,
                       FilterRequest) {
  // ui_test_utils::NavigateToURL(browser(), download_url());
  // DownloadAndWait(download_url());
  ui_test_utils::DownloadURL(browser(), download_url());
  // TODO: accept the download without user interaction

  const std::vector<std::unique_ptr<safe_browsing::ClientDownloadRequest>>&
      requests = safe_browsing::WebUIInfoSingleton::GetInstance()
                     ->client_download_requests_sent();

  ASSERT_EQ(requests.size(), 1u);

  EXPECT_TRUE(requests[0]->has_url());
  EXPECT_EQ(requests[0]->url(), "");
  EXPECT_FALSE(requests[0]->has_locale());
  EXPECT_FALSE(requests[0]->has_file_basename());
  EXPECT_EQ(requests[0]->referrer_chain_size(), 0);
  EXPECT_EQ(requests[0]->resources_size(), 0);
}
