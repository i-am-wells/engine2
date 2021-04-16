#ifndef TOOLS_TILEMAPEDITOR_RECTANGLE_SELECTION_H_
#define TOOLS_TILEMAPEDITOR_RECTANGLE_SELECTION_H_

#include "engine2/point.h"
#include "engine2/rect.h"

namespace tilemapeditor {

class RectangleSelection {
 public:
  void Start(const engine2::Point<>& point);
  void Update(const engine2::Point<>& point);

  engine2::Rect<> GetRect() const;
  void SetRect(const engine2::Rect<>& rect);

 private:
  engine2::Point<> points_[2]{};
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_RECTANGLE_SELECTION_H_