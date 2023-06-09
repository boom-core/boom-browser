// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_FEATURE_ENGAGEMENT_FEATURE_ENGAGEMENT_APP_INTERFACE_H_
#define IOS_CHROME_BROWSER_FEATURE_ENGAGEMENT_FEATURE_ENGAGEMENT_APP_INTERFACE_H_

#import <Foundation/Foundation.h>

#include "base/compiler_specific.h"

// Test specific helpers for feature_engagement_egtest.mm.
@interface FeatureEngagementAppInterface : NSObject

// Resets feature state. Must be called to clean up state created by +enable*
// methods.
+ (void)reset;

// Simulates feature_engagement::events::kChromeOpened.
+ (void)simulateChromeOpenedEvent;

// Enables the Badged Reading List help feature. Clients must call +reset after
// the test finish running. Returns NO if FeatureEngagementTracker failed to
// load.
+ (BOOL)enableBadgedReadingListTriggering [[nodiscard]];

// Enables the Badged Translate Manual Trigger feature. Clients must call +reset
// after the test finish running. Returns NO if FeatureEngagementTracker failed
// to load.
+ (BOOL)enableBadgedTranslateManualTrigger [[nodiscard]];

// Enables the New Tab Tip to be triggered. Clients must call +reset after the
// test finish running. Returns NO if FeatureEngagementTracker failed to load.
+ (BOOL)enableNewTabTipTriggering [[nodiscard]];

// Enables the Bottom Toolbar Tip to be triggered. Clients must call +reset
// after the test finish running. Returns NO if FeatureEngagementTracker failed
// to load.
+ (BOOL)enableBottomToolbarTipTriggering [[nodiscard]];

// Enables the Long Press Tip to be triggered. Clients must call +reset
// after the test finish running. The tip has a configuration where it can be
// displayed as first or second tip of the session and needs to be displayed
// after the BottomToolbar tip is displayed. Returns NO if
// FeatureEngagementTracker failed to load.
+ (BOOL)enableLongPressTipTriggering [[nodiscard]];

// Enables the Default Site View tip to be triggered. The tip is triggered only
// once after requesting the desktop version 3 times. Returns NO if
// FeatureEngagementTracker failed to load.
+ (BOOL)enableDefaultSiteViewTipTriggering [[nodiscard]];

// Enables the Overflow Menu tip to be triggered. The tip is triggered
// after the user has opened the overflow menu twice and not scrolled. Returns
// NO if FeatureEngagementTracker failed to load.
+ (BOOL)enableOverflowMenuTipTriggering [[nodiscard]];

// Enables the Tab Pinned tip to be triggered. The tip is triggered after the
// user pins a tab from the overflow menu.
+ (BOOL)enableTabPinnedTipTriggering [[nodiscard]];

// Starts manual page translation.
+ (void)showTranslate;

// Shows Reading List UI.
+ (void)showReadingList;

@end

#endif  // IOS_CHROME_BROWSER_FEATURE_ENGAGEMENT_FEATURE_ENGAGEMENT_APP_INTERFACE_H_
