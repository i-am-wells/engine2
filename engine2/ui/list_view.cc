#include "engine2/ui/list_view.h"

namespace engine2 {
namespace ui {

ListView::ListView(Direction direction) : direction_(direction) {}

void ListView::AddChild(View* child) {
  children_.push_back(child);
  ChangeSize(child->GetRect().size);
}

// TODO where do child positions come from? they may change here
void ListView::RemoveChild(int index) {
  if (index < 0 || index >= children_.size())
    return;

  ChangeSize(-children_[index]->GetRect().size);
  children_.erase(children_.begin() + index);
}

void ListView::ChangeSize(const Point<int, 2>& child_size) {
  switch (direction_) {
    case Direction::kHorizontal:
      rect_.w() += child_size.x();
      break;
    case Direction::kVertical:
      rect_.h() += child_size.y();
      break;
  }
}

void ListView::SetPosition(const Point<int, 2>& position) {
  rect_.pos = position;
  UpdateChildPositions(0);
}

Rect<int, 2> ListView::GetRect() const {
  return rect_;
}

void ListView::Draw() const {
  for (View* child : children_)
    child->Draw();
}

void ListView::OnMouseEnter(const SDL_MouseMotionEvent& event) {
  View* child = FindChild({event.x, event.y});
  if (!child)
    return;

  child->OnMouseEnter(event);
}

void ListView::OnMouseLeave(const SDL_MouseMotionEvent& event) {
  View* child = FindChild({event.x - event.xrel, event.y - event.yrel});
  if (!child)
    return;

  child->OnMouseLeave(event);
}

void ListView::OnMouseDown(const SDL_MouseButtonEvent& event) {
  View* child = FindChild({event.x, event.y});
  if (!child)
    return;

  child->OnMouseDown(event);
}

void ListView::OnMouseUp(const SDL_MouseButtonEvent& event) {
  View* child = FindChild({event.x, event.y});
  if (!child)
    return;

  child->OnMouseUp(event);
}

View* ListView::FindChild(const Point<int, 2>& point) {
  for (View* child : children_) {
    if (child->GetRect().Contains(point))
      return child;
  }
  return nullptr;
}

void ListView::UpdateChildPositions(int start_index) {
  Point<int, 2> pos = rect_.pos;
  for (auto iter = children_.begin() + start_index; iter != children_.end();
       ++iter) {
    View* child = *iter;
    child->SetPosition(pos);

    switch (direction_) {
      case Direction::kHorizontal:
        pos.x() += child->GetRect().x();
        break;
      case Direction::kVertical:
        pos.y() += child->GetRect().y();
        break;
    }
  }
}

}  // namespace ui
}  // namespace engine2