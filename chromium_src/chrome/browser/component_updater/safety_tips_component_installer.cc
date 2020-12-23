/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#define RegisterSafetyTipsComponent RegisterSafetyTipsComponent_ChromiumImpl
#include "../../../../../chrome/browser/component_updater/safety_tips_component_installer.cc"  // NOLINT
#undef RegisterSafetyTipsComponent

#include "chrome/browser/component_updater/component_updater_utils.h"
#include "components/component_updater/component_updater_service.h"

namespace component_updater {

const char kSafetyTipsComponentId[] = "jflookgnkcckhobaglndicnbbgbonegd";

void OnSafetyTipsRegistered() {
  component_updater::BraveOnDemandUpdate(kSafetyTipsComponentId);
}

void RegisterSafetyTipsComponent(ComponentUpdateService* cus) {
  auto installer = base::MakeRefCounted<ComponentInstaller>(
      std::make_unique<SafetyTipsComponentInstallerPolicy>());
  installer->Register(cus, base::Bind(&OnSafetyTipsRegistered));
}

}  // namespace component_updater
