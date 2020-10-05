#ifndef ENGINE2_RECT_H_
#define ENGINE2_RECT_H_

#include <cstdint>

#include "engine2/point.h"

namespace engine2 {

struct Rect {
 public:
  union {
    struct {
      Point pos, size;
    };
    struct {
      int64_t x, y, w, h;
    };
  };
  bool Touches(const Rect& other) const;
  bool Overlaps(const Rect& other) const;
  bool Contains(const Point& point) const;
  bool Contains(const Rect& other) const;

  // If *this doesn't overlap with other, the returned width and/or height will
  // be 0.
  Rect GetOverlap(const Rect& other) const;

  // Corners:
  // 0----1
  // |    |
  // 2----3
  Point Corner(int i) const;
};

}  // namespace engine2

#endif  // ENGINE2_RECT_H_