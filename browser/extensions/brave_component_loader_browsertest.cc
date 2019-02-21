/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/extensions/brave_component_loader.h"
#include "brave/browser/extensions/brave_extension_functional_test.h"
#include "brave/common/brave_switches.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "content/public/test/browser_test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

using extensions::BraveComponentLoader;

class BravePDFExtensionTest : public extensions::ExtensionFunctionalTest,
                              public BraveComponentLoader::TestingCallbacks {
 public:
  BravePDFExtensionTest() : pdf_extension_action_(TestingCallbacks::NONE) {}
  ~BravePDFExtensionTest() override = default;

protected:
  void SetUpOnMainThread() override {
   extensions::ExtensionService* service =
       extensions::ExtensionSystem::Get(profile())->extension_service();
    DCHECK(service);
   (static_cast<BraveComponentLoader*>(service->component_loader()))
       ->set_testing_callbacks(this);
  }

  // BraveComponentLoader::TestingCallbacks
  void OnPdfExtensionAction(
      TestingCallbacks::PdfExtensionAction action) override {
    pdf_extension_action_ = action;
  }

  void SetDownloadPDFs(bool value) {
    DCHECK(browser());
    profile()->GetPrefs()->SetBoolean(prefs::kPluginsAlwaysOpenPdfExternally,
                                      value);
  }

  TestingCallbacks::PdfExtensionAction pdf_extension_action() {
    return pdf_extension_action_;
  }

private:
  TestingCallbacks::PdfExtensionAction pdf_extension_action_;
};

IN_PROC_BROWSER_TEST_F(BravePDFExtensionTest, ToggleDownloadPDFs) {
  // Set preference to always download PDFs.
  SetDownloadPDFs(true);
  EXPECT_EQ(TestingCallbacks::WILL_REMOVE, pdf_extension_action());

  // Toggle the preference to view PDFs in the browser.
  SetDownloadPDFs(false);
  EXPECT_EQ(TestingCallbacks::WILL_ADD, pdf_extension_action());
}

class BravePDFExtensionDisabledTest : public BravePDFExtensionTest {
public:
  BravePDFExtensionDisabledTest() = default;
  ~BravePDFExtensionDisabledTest() override = default;

protected:
  void SetUpCommandLine(base::CommandLine* command_line) override {
    ExtensionFunctionalTest::SetUpCommandLine(command_line);
    // Disable loading of our PDF extension.
    command_line->AppendSwitch(switches::kDisablePDFJSExtension);
  }
};

IN_PROC_BROWSER_TEST_F(BravePDFExtensionDisabledTest, ToggleDownloadPDFs) {
  // Set preference to always download PDFs.
  SetDownloadPDFs(true);
  EXPECT_EQ(TestingCallbacks::WILL_REMOVE, pdf_extension_action());

  // Toggle the preference to view PDFs in the browser.
  SetDownloadPDFs(false);
  EXPECT_EQ(TestingCallbacks::WILL_REMOVE, pdf_extension_action());
}
