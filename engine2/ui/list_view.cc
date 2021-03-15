#include <cmath>

#include "engine2/ui/list_view.h"

namespace engine2 {
namespace ui {

ListView::ListView(Direction direction,
                   const Vec<int, 2>& padding,
                   const Vec<int, 2>& margin)
    : ContainerView(padding, margin), direction_(direction) {}

void ListView::Relayout() {
  std::vector<HierarchyView*> temp_children = children_;
  children_.clear();
  inner_size_ = {};
  AddChildren(temp_children);
}

void ListView::AddChildren(const std::vector<HierarchyView*>& children) {
  for (HierarchyView* child : children) {
    children_.push_back(child);
    AddChildInternal(child);
  }
  OnSizeChanged();
}

void ListView::AddChild(HierarchyView* child) {
  children_.push_back(child);
  AddChildInternal(child);
  OnSizeChanged();
}

void ListView::InsertChild(HierarchyView* child, int index) {
  if (index < 0 || index > children_.size())
    return;

  children_.insert(children_.begin() + index, child);
  Relayout();
  OnSizeChanged();
}

void ListView::Clear() {
  for (HierarchyView* child : children_)
    child->SetParent(nullptr);

  children_.clear();
  inner_size_ = {};
  OnSizeChanged();
}

void ListView::AddChildInternal(HierarchyView* child) {
  child->SetParent(this);

  Vec<int, 2> child_margin = child->GetMargin();
  Vec<int, 2> child_size = child->GetSize();
  Vec<int, 2> padding = GetPadding();

  Point<int, 2> child_pos;
  switch (direction_) {
    case Direction::kHorizontal:
      child_pos = {inner_size_.x(), padding.y() + child_margin.y()};

      inner_size_.y() =
          std::max(inner_size_.y(), child_size.y() + (child_margin.y() * 2));
      inner_size_.x() += child_size.x() + child_margin.x();

      // Put just one child margin between children (but the first child should
      // have a margin before and after).
      if (children_.size() == 1) {
        child_pos.x() += child_margin.x();
        inner_size_.x() += child_margin.x();
      }
      break;
    case Direction::kVertical:
      child_pos = {padding.x() + child_margin.x(), inner_size_.y()};

      inner_size_.x() =
          std::max(inner_size_.x(), child_size.x() + (child_margin.x() * 2));
      inner_size_.y() += child_size.y() + child_margin.y();

      if (children_.size() == 1) {
        child_pos.y() += child_margin.y();
        inner_size_.y() += child_margin.y();
      }

      break;
  }
  child->SetRelativePosition(child_pos);
}

void ListView::RemoveChild(int index) {
  if (index < 0 || index >= children_.size())
    return;

  children_[index]->SetParent(nullptr);
  children_.erase(children_.begin() + index);
  Relayout();
  OnSizeChanged();
}

}  // namespace ui
}  // namespace engine2