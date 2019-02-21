/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_DAT_FILE_UTIL_H_
#define BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_DAT_FILE_UTIL_H_

#include <vector>

#include "base/callback_forward.h"

namespace base {
class FilePath;
}

namespace brave_shields {

using DATFileDataBuffer = std::vector<unsigned char>;

void GetDATFileData(const base::FilePath& file_path,
                    DATFileDataBuffer* buffer);

}  // namespace brave_shields

#endif  // BRAVE_COMPONENTS_BRAVE_SHIELDS_BROWSER_DAT_FILE_UTIL_H_
