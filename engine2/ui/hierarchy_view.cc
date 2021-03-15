#include "engine2/ui/hierarchy_view.h"

namespace engine2 {
namespace ui {
namespace {

inline constexpr Vec<int, 2> kZeroVec{};

}  // namespace

HierarchyView::HierarchyView(const Vec<int, 2>& padding,
                             const Vec<int, 2>& margin)
    : padding_(padding), margin_(margin) {}

Point<int, 2> HierarchyView::GetRelativePosition() const {
  return relative_pos_;
}
void HierarchyView::SetRelativePosition(const Point<int, 2>& pos) {
  relative_pos_ = pos;
}

Point<int, 2> HierarchyView::GetAbsolutePosition() const {
  if (parent_)
    return parent_->GetAbsolutePosition() + relative_pos_;
  return relative_pos_;
}

Point<int, 2> HierarchyView::GetAbsoluteInnerPosition() const {
  return GetAbsolutePosition() + GetPadding();
}

Vec<int, 2> HierarchyView::GetSize() const {
  return inner_size_ + (GetPadding() * 2);
}

void HierarchyView::SetSize(const Vec<int, 2>& size) {
  inner_size_ = size - (GetPadding() * 2);
  OnSizeChanged();
}

Rect<int, 2> HierarchyView::GetRect() const {
  return {GetAbsolutePosition(), GetSize()};
}

void HierarchyView::OnSizeChanged() {
  if (parent_)
    parent_->OnSizeChanged();
}

Vec<int, 2> HierarchyView::GetMargin() const {
  return margin_;
}

Vec<int, 2> HierarchyView::GetPadding() const {
  return padding_;
}

}  // namespace ui
}  // namespace engine2