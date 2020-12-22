/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#define RegisterSSLErrorAssistantComponent \
  RegisterSSLErrorAssistantComponent_ChromiumImpl
#include "../../../../../chrome/browser/component_updater/ssl_error_assistant_component_installer.cc"  // NOLINT
#undef RegisterSSLErrorAssistantComponent

#include "chrome/browser/component_updater/component_updater_utils.h"
#include "components/component_updater/component_updater_service.h"

namespace component_updater {

const char kSSLErrorAssistantComponentId[] = "giekcmmlnklenlaomppkphknjmnnpneh";

void OnSSLErrorAssistantRegistered() {
  component_updater::BraveOnDemandUpdate(kSSLErrorAssistantComponentId);
}

void RegisterSSLErrorAssistantComponent(ComponentUpdateService* cus) {
  auto installer = base::MakeRefCounted<ComponentInstaller>(
      std::make_unique<SSLErrorAssistantComponentInstallerPolicy>());
  installer->Register(cus, base::Bind(&OnSSLErrorAssistantRegistered));
}

}  // namespace component_updater
