/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#define RegisterFileTypePoliciesComponent RegisterFileTypePoliciesComponent_ChromiumImpl  // NOLINT
#include "../../../../../chrome/browser/component_updater/file_type_policies_component_installer.cc"  // NOLINT
#undef RegisterFileTypePoliciesComponent

#include "chrome/browser/component_updater/component_updater_utils.h"
#include "components/component_updater/component_updater_service.h"
#include "extensions/common/constants.h"

namespace component_updater {

void OnFileTypePoliciesRegistered() {
  component_updater::BraveOnDemandUpdate(file_type_policies_extension_id);
}

void RegisterFileTypePoliciesComponent(ComponentUpdateService* cus,
                                       const base::FilePath& user_data_dir) {
  auto installer = base::MakeRefCounted<ComponentInstaller>(
      std::make_unique<FileTypePoliciesComponentInstallerPolicy>());
  installer->Register(cus, base::Bind(&OnFileTypePoliciesRegistered));
}

}  // namespace component_updater
