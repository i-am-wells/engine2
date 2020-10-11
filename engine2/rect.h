#ifndef ENGINE2_RECT_H_
#define ENGINE2_RECT_H_

#include <cstdint>

#include "engine2/point.h"

namespace engine2 {

template <typename T = int64_t>
struct Rect {
 public:
  union {
    struct {
      Point<T> pos, size;
    };
    struct {
      T x, y, w, h;
    };
  };
  bool Touches(const Rect& other) const;
  bool Overlaps(const Rect& other) const;
  bool Contains(const Point<T>& point) const;
  bool Contains(const Rect& other) const;

  // If *this doesn't overlap with other, the returned width and/or height will
  // be 0.
  Rect GetOverlap(const Rect& other) const;

  // Corners:
  // 0----1
  // |    |
  // 2----3
  Point<T> Corner(int i) const;
};

}  // namespace engine2

#include "engine2/rect.cc"

#endif  // ENGINE2_RECT_H_