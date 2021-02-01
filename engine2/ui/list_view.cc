#include <cmath>

#include "engine2/ui/list_view.h"

namespace engine2 {
namespace ui {

ListView::ListView(Direction direction, const Vec<int, 2>& padding)
    : direction_(direction) {
  SetInitialSize(padding);
}

void ListView::SetInitialSize(const Vec<int, 2>& padding) {
  size_ = padding * 2;
}

void ListView::Relayout() {
  std::vector<HierarchyView*> temp_children = children_;
  children_.clear();
  SetInitialSize(GetPadding());
  AddChildren(temp_children);
}

void ListView::AddChildren(const std::vector<HierarchyView*>& children) {
  for (HierarchyView* child : children)
    AddChildInternal(child);
  OnSizeChanged();
}

void ListView::AddChild(HierarchyView* child) {
  AddChildInternal(child);
  OnSizeChanged();
}

void ListView::AddChildInternal(HierarchyView* child) {
  child->SetParent(this);
  children_.push_back(child);

  Vec<int, 2> child_margin = child->GetMargin();
  Vec<int, 2> child_size = child->GetSize();
  Vec<int, 2> padding = GetPadding();

  Point<int, 2> child_pos;
  switch (direction_) {
    case Direction::kHorizontal:
      child_pos = {size_.x() - padding.x(), padding.y() + child_margin.y()};

      size_.y() = std::max(size_.y(), child_size.y() + (child_margin.y() * 2) +
                                          (padding.y() * 2));
      size_.x() += child_size.x() + child_margin.x();

      // Put just one child margin between children (but the first child should
      // have a margin before and after).
      if (children_.size() == 1) {
        child_pos.x() += child_margin.x();
        size_.x() += child_margin.x();
      }
      break;
    case Direction::kVertical:
      child_pos = {padding.x() + child_margin.x(), size_.y() - padding.y()};

      size_.x() = std::max(size_.x(), child_size.x() + (child_margin.x() * 2) +
                                          (padding.x() * 2));
      size_.y() += child_size.y() + child_margin.y();

      if (children_.size() == 1) {
        child_pos.y() += child_margin.y();
        size_.y() += child_margin.y();
      }

      break;
  }
  child->SetRelativePosition(child_pos);
}

void ListView::RemoveChild(int index) {
  if (index < 0 || index >= children_.size())
    return;

  children_.erase(children_.begin() + index);
  Relayout();
  OnSizeChanged();
}

}  // namespace ui
}  // namespace engine2