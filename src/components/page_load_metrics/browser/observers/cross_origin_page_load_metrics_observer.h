// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PAGE_LOAD_METRICS_BROWSER_OBSERVERS_CROSS_ORIGIN_PAGE_LOAD_METRICS_OBSERVER_H_
#define COMPONENTS_PAGE_LOAD_METRICS_BROWSER_OBSERVERS_CROSS_ORIGIN_PAGE_LOAD_METRICS_OBSERVER_H_

#include "components/page_load_metrics/browser/page_load_metrics_observer.h"

class CrossOriginPageLoadMetricsObserver
    : public page_load_metrics::PageLoadMetricsObserver {
 public:
  CrossOriginPageLoadMetricsObserver() = default;

  CrossOriginPageLoadMetricsObserver(
      const CrossOriginPageLoadMetricsObserver&) = delete;
  CrossOriginPageLoadMetricsObserver& operator=(
      const CrossOriginPageLoadMetricsObserver&) = delete;

 private:
  // page_load_metrics::PageLoadMetricsObserver implementation:
  ObservePolicy OnFencedFramesStart(
      content::NavigationHandle* navigation_handle,
      const GURL& currently_committed_url) override;
  ObservePolicy OnPrerenderStart(content::NavigationHandle* navigation_handle,
                                 const GURL& currently_committed_url) override;
  ObservePolicy OnCommit(content::NavigationHandle* navigation_handle) override;
  void OnFirstInputInPage(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnFirstContentfulPaintInPage(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnComplete(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  page_load_metrics::PageLoadMetricsObserver::ObservePolicy
  FlushMetricsOnAppEnterBackground(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void RecordTimingHistograms();
};

#endif  // COMPONENTS_PAGE_LOAD_METRICS_BROWSER_OBSERVERS_CROSS_ORIGIN_PAGE_LOAD_METRICS_OBSERVER_H_
