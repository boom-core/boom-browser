// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/view_transition/view_transition_content_element.h"

#include "third_party/blink/renderer/core/layout/layout_view_transition_content.h"

namespace blink {

ViewTransitionContentElement::ViewTransitionContentElement(
    Element* parent,
    PseudoId pseudo_id,
    const AtomicString& view_transition_name,
    viz::ViewTransitionElementResourceId resource_id,
    bool is_live_content_element,
    const ViewTransitionStyleTracker* style_tracker)
    : ViewTransitionPseudoElementBase(parent,
                                      pseudo_id,
                                      view_transition_name,
                                      style_tracker),
      resource_id_(resource_id),
      is_live_content_element_(is_live_content_element) {
  DCHECK(resource_id_.IsValid());
}

ViewTransitionContentElement::~ViewTransitionContentElement() = default;

void ViewTransitionContentElement::SetIntrinsicSize(
    const gfx::RectF& ink_overflow_rect,
    const gfx::RectF& captured_subrect) {
  ink_overflow_rect_ = ink_overflow_rect;
  captured_subrect_ = captured_subrect;
  if (auto* layout_object = GetLayoutObject()) {
    static_cast<LayoutViewTransitionContent*>(layout_object)
        ->OnIntrinsicSizeUpdated(ink_overflow_rect_, captured_subrect_);
  }
}

LayoutObject* ViewTransitionContentElement::CreateLayoutObject(
    const ComputedStyle&,
    LegacyLayout) {
  return MakeGarbageCollected<LayoutViewTransitionContent>(this);
}

}  // namespace blink
