/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef CHROME_COMMON_CHROME_ICON_RESOURCES_WIN_H_
#define CHROME_COMMON_CHROME_ICON_RESOURCES_WIN_H_

namespace icon_resources {

// This file contains the indices of icon resources in brave_exe.rc.

enum {
  // The main application icon is always index 0.
  kApplicationIndex = 0,

#if defined(OFFICIAL_BUILD)
  // Legacy indices that are no longer used.
  kApplication2Index = 1,
  kApplication3Index = 2,
  kApplication4Index = 3,

  // The Brave Canary application icon.
  kSxSApplicationIndex = 4,

  // The Brave App Launcher icon.
  kAppLauncherIndex = 5,

  // The Brave App Launcher Canary icon.
  kSxSAppLauncherIndex = 6,

  // The Brave incognito icon.
  kIncognitoIndex = 7,

  // The Brave Dev application icon.
  kDevApplicationIndex = 8,

  // The Brave Beta application icon.
  kBetaApplicationIndex = 9,
#else
  // The Brave Developer App Launcher icon.
  kAppLauncherIndex = 1,

  // The Brave Developer incognito icon.
  kIncognitoIndex = 2,
#endif
};

}  // namespace icon_resources

#endif  // CHROME_COMMON_CHROME_ICON_RESOURCES_WIN_H_
