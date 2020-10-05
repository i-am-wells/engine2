#include "engine2/rect.h"

#include <algorithm>

namespace engine2 {
namespace {

bool HalfPartialOverlap(const Rect& a, const Rect& b, int d) {
  return (a.pos[d] >= b.pos[d]) && (a.pos[d] < (b.pos[d] + b.size[d]));
}

bool PartialOverlap(const Rect& a, const Rect& b, int d) {
  return HalfPartialOverlap(a, b, d) || HalfPartialOverlap(b, a, d);
}

bool HalfPartialTouch(const Rect& a, const Rect& b, int d) {
  return (a.pos[d] + a.size[d]) == b.pos[d];
}

bool PartialTouch(const Rect& a, const Rect& b, int d) {
  return HalfPartialTouch(a, b, d) || HalfPartialTouch(b, a, d);
}

bool PartialTouchOrOverlap(const Rect& a, const Rect& b, int d) {
  return PartialTouch(a, b, d) || PartialOverlap(a, b, d);
}

void ComputePartialOverlap(const Rect& a, const Rect& b, int d, Rect* result) {
  if (HalfPartialOverlap(a, b, d)) {
    result->pos[d] = a.pos[d];
    result->size[d] = std::min(a.size[d], b.pos[d] + b.size[d] - a.pos[d]);
  } else if (HalfPartialOverlap(b, a, d)) {
    result->pos[d] = b.pos[d];
    result->size[d] = std::min(b.size[d], a.pos[d] + a.size[d] - b.pos[d]);
  }
}

}  // namespace

bool Rect::Overlaps(const Rect& other) const {
  return PartialOverlap(*this, other, 0) && PartialOverlap(*this, other, 1);
}

bool Rect::Touches(const Rect& other) const {
  return (PartialTouch(*this, other, 0) &&
          PartialTouchOrOverlap(*this, other, 1)) ||
         (PartialTouch(*this, other, 1) &&
          PartialTouchOrOverlap(*this, other, 0));
}

bool Rect::Contains(const Point& point) const {
  return point.x >= x && point.x < (x + w) && point.y >= y && point.y < (y + h);
}

bool Rect::Contains(const Rect& other) const {
  return Contains(other.Corner(0)) && Contains(other.Corner(3));
}

Rect Rect::GetOverlap(const Rect& other) const {
  Rect overlap{0, 0, 0, 0};
  ComputePartialOverlap(*this, other, 0, &overlap);
  ComputePartialOverlap(*this, other, 1, &overlap);
  return overlap;
}

Point Rect::Corner(int i) const {
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