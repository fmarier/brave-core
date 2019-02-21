/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_CONTENT_SETTINGS_BRAVE_WIDEVINE_BLOCKED_IMAGE_MODEL_H_
#define BRAVE_BROWSER_UI_CONTENT_SETTINGS_BRAVE_WIDEVINE_BLOCKED_IMAGE_MODEL_H_

#include "chrome/browser/ui/content_settings/content_setting_image_model.h"

class BraveWidevineBlockedImageModel : public ContentSettingSimpleImageModel {
 public:
  BraveWidevineBlockedImageModel(ImageType image_type,
                                               ContentSettingsType content_type);
  bool UpdateAndGetVisibility(content::WebContents* web_contents) override;
  std::unique_ptr<ContentSettingBubbleModel> CreateBubbleModelImpl(
    ContentSettingBubbleModel::Delegate* delegate,
    content::WebContents* web_contents) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(BraveWidevineBlockedImageModel);
};

#endif  // BRAVE_BROWSER_UI_CONTENT_SETTINGS_BRAVE_WIDEVINE_BLOCKED_IMAGE_MODEL_H_
