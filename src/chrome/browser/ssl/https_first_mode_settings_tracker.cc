// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ssl/https_first_mode_settings_tracker.h"

#include "base/feature_list.h"
#include "base/metrics/histogram_functions.h"
#include "build/chromeos_buildflags.h"
#include "chrome/browser/metrics/chrome_metrics_service_accessor.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/safe_browsing/advanced_protection_status_manager_factory.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "components/variations/synthetic_trials.h"
#include "content/public/browser/browser_context.h"

#if BUILDFLAG(IS_CHROMEOS_ASH)
#include "chrome/browser/ash/profiles/profile_helper.h"
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

namespace {
const char kHttpsFirstModeServiceName[] = "HttpsFirstModeService";
const char kHttpsFirstModeSyntheticFieldTrialName[] =
    "HttpsFirstModeClientSetting";
const char kHttpsFirstModeSyntheticFieldTrialEnabledGroup[] = "Enabled";
const char kHttpsFirstModeSyntheticFieldTrialDisabledGroup[] = "Disabled";
}  // namespace

HttpsFirstModeService::HttpsFirstModeService(Profile* profile)
    : profile_(profile) {
  pref_change_registrar_.Init(profile_->GetPrefs());
  // Using base::Unretained() here is safe as the PrefChangeRegistrar is owned
  // by `this`.
  pref_change_registrar_.Add(
      prefs::kHttpsOnlyModeEnabled,
      base::BindRepeating(&HttpsFirstModeService::OnHttpsFirstModePrefChanged,
                          base::Unretained(this)));

  // Track Advanced Protection status.
  if (base::FeatureList::IsEnabled(
          features::kHttpsFirstModeForAdvancedProtectionUsers)) {
    obs_.Observe(
        safe_browsing::AdvancedProtectionStatusManagerFactory::GetForProfile(
            profile_));
    // On startup, AdvancedProtectionStatusManager runs before this class so we
    // don't get called back. Run the callback to get the AP setting.
    OnAdvancedProtectionStatusChanged(
        safe_browsing::AdvancedProtectionStatusManagerFactory::GetForProfile(
            profile_)
            ->IsUnderAdvancedProtection());
  }

  // Make sure the pref state is logged and the synthetic field trial state is
  // created at startup (as the pref may never change over the session).
  bool enabled = profile_->GetPrefs()->GetBoolean(prefs::kHttpsOnlyModeEnabled);
  base::UmaHistogramBoolean("Security.HttpsFirstMode.SettingEnabledAtStartup",
                            enabled);
  ChromeMetricsServiceAccessor::RegisterSyntheticFieldTrial(
      kHttpsFirstModeSyntheticFieldTrialName,
      enabled ? kHttpsFirstModeSyntheticFieldTrialEnabledGroup
              : kHttpsFirstModeSyntheticFieldTrialDisabledGroup,
      variations::SyntheticTrialAnnotationMode::kCurrentLog);
}

HttpsFirstModeService::~HttpsFirstModeService() = default;

void HttpsFirstModeService::OnHttpsFirstModePrefChanged() {
  bool enabled = profile_->GetPrefs()->GetBoolean(prefs::kHttpsOnlyModeEnabled);
  base::UmaHistogramBoolean("Security.HttpsFirstMode.SettingChanged", enabled);
  // Update synthetic field trial group registration.
  ChromeMetricsServiceAccessor::RegisterSyntheticFieldTrial(
      kHttpsFirstModeSyntheticFieldTrialName,
      enabled ? kHttpsFirstModeSyntheticFieldTrialEnabledGroup
              : kHttpsFirstModeSyntheticFieldTrialDisabledGroup);
}

void HttpsFirstModeService::OnAdvancedProtectionStatusChanged(bool enabled) {
  DCHECK(base::FeatureList::IsEnabled(
      features::kHttpsFirstModeForAdvancedProtectionUsers));
  // Override the pref if AP is enabled. We explicitly don't unset the pref if
  // the user is no longer under Advanced Protection.
  if (enabled &&
      !profile_->GetPrefs()->GetBoolean(prefs::kHttpsOnlyModeEnabled)) {
    profile_->GetPrefs()->SetBoolean(prefs::kHttpsOnlyModeEnabled, true);
  }
}

// static
HttpsFirstModeService* HttpsFirstModeServiceFactory::GetForProfile(
    Profile* profile) {
  return static_cast<HttpsFirstModeService*>(
      GetInstance()->GetServiceForBrowserContext(profile, /*create=*/true));
}

// static
HttpsFirstModeServiceFactory* HttpsFirstModeServiceFactory::GetInstance() {
  return base::Singleton<HttpsFirstModeServiceFactory>::get();
}

HttpsFirstModeServiceFactory::HttpsFirstModeServiceFactory()
    : ProfileKeyedServiceFactory(
          kHttpsFirstModeServiceName,
          // Don't create a service for non-regular profiles. This includes
          // Incognito (which uses the settings of the main profile) and Guest
          // Mode.
          ProfileSelections::BuildForRegularProfile()) {
  DependsOn(
      safe_browsing::AdvancedProtectionStatusManagerFactory::GetInstance());
}

HttpsFirstModeServiceFactory::~HttpsFirstModeServiceFactory() = default;

// BrowserContextKeyedServiceFactory:
KeyedService* HttpsFirstModeServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* browser_context) const {
  Profile* profile = Profile::FromBrowserContext(browser_context);
#if BUILDFLAG(IS_CHROMEOS_ASH)
  // Explicitly check for ChromeOS sign-in profiles (which would cause
  // double-counting of at-startup metrics for ChromeOS restarts) which are not
  // covered by the `IsRegularProfile()` check.
  if (ash::ProfileHelper::IsSigninProfile(profile)) {
    return nullptr;
  }
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
  return new HttpsFirstModeService(profile);
}
