// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/web_applications/os_integration/web_app_uninstallation_via_os_settings_registration.h"

#include "base/check.h"
#include "build/build_config.h"

namespace web_app {

#if !BUILDFLAG(IS_WIN)

// This block defines stub implementations of OS specific methods for
// uninstallation command. Currently, only Windows has its own implementation.
bool ShouldRegisterUninstallationViaOsSettingsWithOs() {
  return false;
}

bool RegisterUninstallationViaOsSettingsWithOs(const AppId& app_id,
                                               const std::string& app_name,
                                               Profile* profile) {
  NOTREACHED();
  return true;
}

bool UnregisterUninstallationViaOsSettingsWithOs(const AppId& app_id,
                                                 Profile* profile) {
  NOTREACHED();
  return true;
}

#endif  // !BUILDFLAG(IS_WIN)

}  // namespace web_app
