#include "engine2/ui/hierarchy_view.h"

namespace engine2 {
namespace ui {
namespace {

inline constexpr Vec<int, 2> kZeroVec{};

}  // namespace

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

Vec<int, 2> HierarchyView::GetSize() const {
  return size_;
}

void HierarchyView::SetSize(const Vec<int, 2>& size) {
  size_ = size;
  OnSizeChanged();
}

Rect<int, 2> HierarchyView::GetRect() const {
  return {GetAbsolutePosition(), GetSize()};
}

void HierarchyView::OnSizeChanged() {
  if (parent_)
    parent_->OnSizeChanged();
}

}  // namespace ui
}  // namespace engine2