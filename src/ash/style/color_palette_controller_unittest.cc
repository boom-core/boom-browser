// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/style/color_palette_controller.h"

#include "ash/shell.h"
#include "ash/style/dark_light_mode_controller_impl.h"
#include "ash/test/ash_test_base.h"
#include "ash/wallpaper/wallpaper_controller_impl.h"
#include "base/functional/callback_helpers.h"
#include "third_party/skia/include/core/SkColor.h"

namespace ash {

namespace {

const char kUser[] = "user@gmail.com";
const AccountId kAccountId = AccountId::FromUserEmailGaiaId(kUser, kUser);

}  // namespace

class ColorPaletteControllerTest : public NoSessionAshTestBase {
 public:
  void SetUp() override {
    NoSessionAshTestBase::SetUp();
    GetSessionControllerClient()->Reset();
    GetSessionControllerClient()->AddUserSession(kAccountId, kUser);
    color_palette_controller_ = ColorPaletteController::Create(
        Shell::Get()->dark_light_mode_controller(),
        Shell::Get()->wallpaper_controller());
  }

  void TearDown() override {
    // Must release the controller before Shell is destructed.
    color_palette_controller_.reset();

    NoSessionAshTestBase::TearDown();
  }

  ColorPaletteController* color_palette_controller() {
    return color_palette_controller_.get();
  }

 private:
  std::unique_ptr<ColorPaletteController> color_palette_controller_;
};

TEST_F(ColorPaletteControllerTest, ExpectedEmptyValues) {
  EXPECT_EQ(ColorScheme::kTonalSpot,
            color_palette_controller()->GetColorScheme(kAccountId));
  EXPECT_EQ(absl::nullopt,
            color_palette_controller()->GetStaticColor(kAccountId));
}

TEST_F(ColorPaletteControllerTest, SetColorScheme) {
  ColorScheme color_scheme = ColorScheme::kExpressive;

  color_palette_controller()->SetColorScheme(color_scheme, kAccountId,
                                             base::DoNothing());

  EXPECT_EQ(color_scheme,
            color_palette_controller()->GetColorScheme(kAccountId));
  EXPECT_EQ(absl::nullopt,
            color_palette_controller()->GetStaticColor(kAccountId));
  auto color_palette_seed =
      color_palette_controller()->GetColorPaletteSeed(kAccountId);
  EXPECT_EQ(color_scheme, color_palette_seed.scheme);
}

TEST_F(ColorPaletteControllerTest, SetStaticColor) {
  SkColor static_color = SK_ColorGRAY;

  color_palette_controller()->SetStaticColor(static_color, kAccountId,
                                             base::DoNothing());

  EXPECT_EQ(static_color,
            color_palette_controller()->GetStaticColor(kAccountId));
  EXPECT_EQ(ColorScheme::kStatic,
            color_palette_controller()->GetColorScheme(kAccountId));
  auto color_palette_seed =
      color_palette_controller()->GetColorPaletteSeed(kAccountId);
  EXPECT_EQ(ColorScheme::kStatic, color_palette_seed.scheme);
  EXPECT_EQ(static_color, color_palette_seed.seed_color);
}

}  // namespace ash
