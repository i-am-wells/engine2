#include "engine2/ui/list_view.h"

namespace engine2 {
namespace ui {

ListView::ListView(Direction direction)
    : ContainerView(Rect<int, 2>{}, std::vector<View*>()),
      direction_(direction) {}

void ListView::AddChild(View* child) {
  views_.push_back(child);
  ChangeSize(child->GetRect().size);
}

// TODO where do child positions come from? they may change here
void ListView::RemoveChild(int index) {
  if (index < 0 || index >= views_.size())
    return;

  ChangeSize(-views_[index]->GetRect().size);
  views_.erase(views_.begin() + index);
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

void ListView::UpdateChildPositions(int start_index) {
  Point<int, 2> pos = rect_.pos;
  for (auto iter = views_.begin() + start_index; iter != views_.end(); ++iter) {
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