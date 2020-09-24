/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/brave_adblock_ui.h"

#include "brave/browser/brave_browser_process_impl.h"
#include "brave/common/pref_names.h"
#include "brave/common/webui_url_constants.h"
#include "brave/components/brave_adblock/resources/grit/brave_adblock_generated_map.h"
#include "brave/components/brave_shields/browser/ad_block_custom_filters_service.h"
#include "brave/components/brave_shields/browser/ad_block_regional_service_manager.h"
#include "chrome/browser/profiles/profile.h"
#include "components/grit/brave_components_resources.h"
#include "components/prefs/pref_change_registrar.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/browser/web_ui_message_handler.h"

BraveAdblockUI::BraveAdblockUI(content::WebUI* web_ui, const std::string& name)
    : WebUIController(web_ui) {
  observer_.reset(
      new BasicUIWebContentsObserver(this, web_ui->GetWebContents()));
  Profile* profile = Profile::FromWebUI(web_ui);
  content::WebUIDataSource* source =
      content::WebUIDataSource::Create(name);
  source->SetDefaultResource(html_resource_id);
  // Add generated resource paths
  for (size_t i = 0; i < resource_map_size; ++i) {
    source->AddResourcePath(resource_map[i].name,  resource_map[i].value);
  }
  CustomizeWebUIHTMLSource(name, source);
  return source;

  content::WebUIDataSource::Add(profile, source);
}

BraveAdblockUI::~BraveAdblockUI() {
}
