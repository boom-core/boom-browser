// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_SETTINGS_PRICE_NOTIFICATIONS_PRICE_NOTIFICATIONS_COORDINATOR_H_
#define IOS_CHROME_BROWSER_UI_SETTINGS_PRICE_NOTIFICATIONS_PRICE_NOTIFICATIONS_COORDINATOR_H_

#import "ios/chrome/browser/shared/coordinator/chrome_coordinator/chrome_coordinator.h"

@class PriceNotificationsCoordinator;

// Delegate that allows to dereference the PriceNotificationsCoordinator.
@protocol PriceNotificationsCoordinatorDelegate

// Called when the view controller is removed from navigation controller.
- (void)priceNotificationsCoordinatorDidRemove:
    (PriceNotificationsCoordinator*)coordinator;

@end

// The coordinator for the Price Notifications screen.
@interface PriceNotificationsCoordinator : ChromeCoordinator

@property(nonatomic, weak) id<PriceNotificationsCoordinatorDelegate> delegate;

- (instancetype)initWithBaseViewController:(UIViewController*)viewController
                                   browser:(Browser*)browser NS_UNAVAILABLE;

- (instancetype)initWithBaseNavigationController:
                    (UINavigationController*)navigationController
                                         browser:(Browser*)browser
    NS_DESIGNATED_INITIALIZER;

@end

#endif  // IOS_CHROME_BROWSER_UI_SETTINGS_PRICE_NOTIFICATIONS_PRICE_NOTIFICATIONS_COORDINATOR_H_
