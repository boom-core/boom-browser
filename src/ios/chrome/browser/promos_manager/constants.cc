// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/chrome/browser/promos_manager/constants.h"

#include "base/notreached.h"
#import "third_party/abseil-cpp/absl/types/optional.h"

namespace promos_manager {

const char kImpressionPromoKey[] = "promo";
const char kImpressionDayKey[] = "day";
const int kNumDaysImpressionHistoryStored = 365;

// WARNING - PLEASE READ: Sadly, we cannot switch over strings in C++, so be
// very careful when updating this method to ensure all enums are accounted for.
absl::optional<Promo> PromoForName(base::StringPiece promo) {
  if (promo == "promos_manager::Promo::Test")
    return promos_manager::Promo::Test;

  if (promo == "promos_manager::Promo::DefaultBrowser")
    return promos_manager::Promo::DefaultBrowser;

  if (promo == "promos_manager::Promo::AppStoreRating")
    return promos_manager::Promo::AppStoreRating;

  if (promo == "promos_manager::Promo::CredentialProviderExtension")
    return promos_manager::Promo::CredentialProviderExtension;

  if (promo == "promos_manager::Promo::PostRestoreSignInFullscreen")
    return promos_manager::Promo::PostRestoreSignInFullscreen;

  if (promo == "promos_manager::Promo::PostRestoreSignInAlert")
    return promos_manager::Promo::PostRestoreSignInAlert;

  if (promo == "promos_manager::Promo::WhatsNew")
    return promos_manager::Promo::WhatsNew;

  return absl::nullopt;
}

base::StringPiece NameForPromo(Promo promo) {
  switch (promo) {
    case promos_manager::Promo::Test:
      return "promos_manager::Promo::Test";
    case promos_manager::Promo::DefaultBrowser:
      return "promos_manager::Promo::DefaultBrowser";
    case promos_manager::Promo::AppStoreRating:
      return "promos_manager::Promo::AppStoreRating";
    case promos_manager::Promo::CredentialProviderExtension:
      return "promos_manager::Promo::CredentialProviderExtension";
    case promos_manager::Promo::PostRestoreSignInFullscreen:
      return "promos_manager::Promo::PostRestoreSignInFullscreen";
    case promos_manager::Promo::PostRestoreSignInAlert:
      return "promos_manager::Promo::PostRestoreSignInAlert";
    case promos_manager::Promo::WhatsNew:
      return "promos_manager::Promo::WhatsNew";
  }
}

}  // namespace promos_manager
