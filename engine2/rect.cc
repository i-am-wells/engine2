//#include "engine2/rect.h"

#include <algorithm>

namespace engine2 {
namespace {

template <typename T>
bool HalfPartialOverlap(const Rect<T>& a, const Rect<T>& b, int d) {
  return (a.pos[d] >= b.pos[d]) && (a.pos[d] < (b.pos[d] + b.size[d]));
}

template <typename T>
bool PartialOverlap(const Rect<T>& a, const Rect<T>& b, int d) {
  return HalfPartialOverlap(a, b, d) || HalfPartialOverlap(b, a, d);
}

template <typename T>
bool HalfPartialTouch(const Rect<T>& a, const Rect<T>& b, int d) {
  return (a.pos[d] + a.size[d]) == b.pos[d];
}

template <typename T>
bool PartialTouch(const Rect<T>& a, const Rect<T>& b, int d) {
  return HalfPartialTouch(a, b, d) || HalfPartialTouch(b, a, d);
}

template <typename T>
bool PartialTouchOrOverlap(const Rect<T>& a, const Rect<T>& b, int d) {
  return PartialTouch(a, b, d) || PartialOverlap(a, b, d);
}

template <typename T>
void ComputePartialOverlap(const Rect<T>& a,
                           const Rect<T>& b,
                           int d,
                           Rect<T>* result) {
  if (HalfPartialOverlap(a, b, d)) {
    result->pos[d] = a.pos[d];
    result->size[d] = std::min(a.size[d], b.pos[d] + b.size[d] - a.pos[d]);
  } else if (HalfPartialOverlap(b, a, d)) {
    result->pos[d] = b.pos[d];
    result->size[d] = std::min(b.size[d], a.pos[d] + a.size[d] - b.pos[d]);
  }
}

}  // namespace

template <typename T>
bool Rect<T>::Overlaps(const Rect<T>& other) const {
  return PartialOverlap(*this, other, 0) && PartialOverlap(*this, other, 1);
}

template <typename T>
bool Rect<T>::Touches(const Rect<T>& other) const {
  return (PartialTouch(*this, other, 0) &&
          PartialTouchOrOverlap(*this, other, 1)) ||
         (PartialTouch(*this, other, 1) &&
          PartialTouchOrOverlap(*this, other, 0));
}

template <typename T>
bool Rect<T>::Contains(const Point<T>& point) const {
  return point.x >= x && point.x < (x + w) && point.y >= y && point.y < (y + h);
}

template <typename T>
bool Rect<T>::Contains(const Rect<T>& other) const {
  return Contains(other.Corner(0)) && Contains(other.Corner(3));
}

template <typename T>
Rect<T> Rect<T>::GetOverlap(const Rect<T>& other) const {
  Rect<T> overlap{0, 0, 0, 0};
  ComputePartialOverlap(*this, other, 0, &overlap);
  ComputePartialOverlap(*this, other, 1, &overlap);
  return overlap;
}

template <typename T>
Point<T> Rect<T>::Corner(int i) const {
  switch (i) {
    case 1:
      return {x + w - 1, y};
    case 2:
      return {x, y + h - 1};
    case 3:
      return {x + w - 1, y + h - 1};
    case 0:
    default:
      return {x, y};
  }
}

}  // namespace engine2