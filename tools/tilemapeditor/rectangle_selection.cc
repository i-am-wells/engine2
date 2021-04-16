#include <algorithm>

#include "tools/tilemapeditor/rectangle_selection.h"

using engine2::Point;
using engine2::Rect;
using engine2::Vec;

namespace tilemapeditor {

void RectangleSelection::Start(const Point<>& point) {
  points_[0] = point;
  points_[1] = point;
}

void RectangleSelection::Update(const Point<>& point) {
  points_[1] = point;
}

Rect<> RectangleSelection::GetRect() const {
  Vec<int64_t, 2> diff = points_[1] - points_[0];
  return {std::min(points_[0].x(), points_[1].x()),
          std::min(points_[0].y(), points_[1].y()), std::abs(diff.x()) + 1,
          std::abs(diff.y()) + 1};
}

void RectangleSelection::SetRect(const Rect<>& rect) {
  points_[0] = rect.pos;
  points_[1] = rect.pos + rect.size - 1l;
}

}  // namespace tilemapeditor