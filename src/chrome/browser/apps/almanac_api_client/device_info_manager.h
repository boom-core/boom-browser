// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_APPS_ALMANAC_API_CLIENT_DEVICE_INFO_MANAGER_H_
#define CHROME_BROWSER_APPS_ALMANAC_API_CLIENT_DEVICE_INFO_MANAGER_H_

#include <ostream>
#include <string>

#include "base/functional/callback_forward.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/system/sys_info.h"
#include "components/version_info/channel.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

class Profile;

namespace apps {

struct VersionInfo {
  // The ash Chrome browser version of the device. e.g. "107.0.5296.0"
  std::string ash_chrome;
  // The ChromeOS platform version of the device. e.g. "15088.0.0"
  // The value is set to "unknown" if the version was not known.
  std::string platform;
  // The channel of the build.
  version_info::Channel channel = version_info::Channel::UNKNOWN;
};

struct DeviceInfo {
  DeviceInfo();
  DeviceInfo(const DeviceInfo& other);
  DeviceInfo& operator=(const DeviceInfo& other);
  ~DeviceInfo();

  // The board family of the device. e.g. "brya"
  std::string board;

  // The model of the device. e.g. "taniks"
  std::string model;

  // The user type of the profile currently running. e.g. "unmanaged"
  std::string user_type;

  // The version info of the device.
  VersionInfo version_info;

  // The locale chosen by the user (e.g. "en-AU"). If no user preference is
  // available, which happens during OOBE, instead falls back to the language
  // the UI is currently showing in. This may be less specific (e.g. "en-GB"
  // instead of "en-AU"), or may conflict with sync data (e.g. signing into a
  // "fr" device with an account that has "de" in prefs).
  std::string locale;
};

// Fetches information about the device the code is currently running on, used
// to populate the device context for requests to the Almanac API server.
class DeviceInfoManager {
 public:
  explicit DeviceInfoManager(Profile* profile);
  DeviceInfoManager(const DeviceInfoManager&) = delete;
  DeviceInfoManager& operator=(const DeviceInfoManager&) = delete;
  ~DeviceInfoManager();

  // Asynchronously fetches device information. Must be called from the UI
  // thread.
  void GetDeviceInfo(base::OnceCallback<void(DeviceInfo)> callback);

 private:
  void OnPlatformVersionNumber(base::OnceCallback<void(DeviceInfo)> callback,
                               DeviceInfo device_info,
                               const absl::optional<std::string>& version);
  void OnModelInfo(base::OnceCallback<void(DeviceInfo)> callback,
                   DeviceInfo device_info,
                   base::SysInfo::HardwareInfo hardware_info);

  base::raw_ptr<Profile> profile_;
  absl::optional<DeviceInfo> device_info_ = absl::nullopt;

  // |weak_ptr_factory_| must be the last member of this class.
  base::WeakPtrFactory<DeviceInfoManager> weak_ptr_factory_{this};
};

std::ostream& operator<<(std::ostream& os, const DeviceInfo& device_info);

std::ostream& operator<<(std::ostream& os, const VersionInfo& version_info);

}  // namespace apps

#endif  // CHROME_BROWSER_APPS_ALMANAC_API_CLIENT_DEVICE_INFO_MANAGER_H_
