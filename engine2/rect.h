#ifndef ENGINE2_RECT_H_
#define ENGINE2_RECT_H_

#include <algorithm>
#include <cstdint>
#include <initializer_list>

#include "engine2/point.h"

namespace engine2 {

template <typename Scalar = int64_t, int N = 2>
struct Rect {
  Point<Scalar, N> pos, size;

  Scalar& x() { return pos.x(); }
  const Scalar& x() const { return pos.x(); }
  Scalar& y() { return pos.y(); }
  const Scalar& y() const { return pos.y(); }
  Scalar& z() { return pos.z(); }
  const Scalar& z() const { return pos.z(); }
  Scalar& a() { return pos.a(); }
  const Scalar& a() const { return pos.a(); }

  Scalar& w() { return size.x(); }
  const Scalar& w() const { return size.x(); }
  Scalar& h() { return size.y(); }
  const Scalar& h() const { return size.y(); }
  Scalar& d() { return size.z(); }
  const Scalar& d() const { return size.z(); }
  Scalar& a_size() { return size.a(); }
  const Scalar& a_size() const { return size.a(); }

  template <typename OtherScalar>
  Rect<OtherScalar, N> ConvertTo() const {
    return {pos.template ConvertTo<OtherScalar>(),
            size.template ConvertTo<OtherScalar>()};
  }

  bool operator==(const Rect& other) const {
    return pos == other.pos && size == other.size;
  }

  bool Touches(const Rect& other) const;
  bool Overlaps(const Rect& other) const;
  bool Contains(const Point<Scalar, N>& point) const;
  bool Contains(const Rect& other) const;

  // If *this doesn't overlap with other, the returned width and/or height will
  // be 0.
  Rect GetOverlap(const Rect& other) const;

  // Returns the index of the axis along which *this and other are touching. If
  // !Touches(other), return -1. If touching in multiple dimensions, returns the
  // lowest index.
  int GetTouchingDimension(const Rect& other) const;
};

// TODO: maybe these should be members of Rect (but with better names)
namespace rect_internal {

template <typename Scalar, int N>
bool HalfPartialOverlap(const Rect<Scalar, N>& a,
                        const Rect<Scalar, N>& b,
                        int d) {
  return (a.pos[d] >= b.pos[d]) && (a.pos[d] < (b.pos[d] + b.size[d]));
}

template <typename Scalar, int N>
bool PartialOverlap(const Rect<Scalar, N>& a, const Rect<Scalar, N>& b, int d) {
  return HalfPartialOverlap(a, b, d) || HalfPartialOverlap(b, a, d);
}

template <typename Scalar, int N>
bool HalfPartialTouch(const Rect<Scalar, N>& a,
                      const Rect<Scalar, N>& b,
                      int d) {
  return (a.pos[d] + a.size[d]) == b.pos[d];
}

template <typename Scalar, int N>
bool PartialTouch(const Rect<Scalar, N>& a, const Rect<Scalar, N>& b, int d) {
  return HalfPartialTouch(a, b, d) || HalfPartialTouch(b, a, d);
}

template <typename Scalar, int N>
bool PartialTouchOrOverlap(const Rect<Scalar, N>& a,
                           const Rect<Scalar, N>& b,
                           int d) {
  return PartialTouch(a, b, d) || PartialOverlap(a, b, d);
}

template <typename Scalar, int N>
void ComputePartialOverlap(const Rect<Scalar, N>& a,
                           const Rect<Scalar, N>& b,
                           int d,
                           Rect<Scalar, N>* result) {
  result->size[d] = 0;
  if (HalfPartialOverlap(a, b, d)) {
    result->pos[d] = a.pos[d];
    result->size[d] = std::min(a.size[d], b.pos[d] + b.size[d] - a.pos[d]);
  } else if (HalfPartialOverlap(b, a, d)) {
    result->pos[d] = b.pos[d];
    result->size[d] = std::min(b.size[d], a.pos[d] + a.size[d] - b.pos[d]);
  }
}

}  // namespace rect_internal

template <typename Scalar, int N>
bool Rect<Scalar, N>::Overlaps(const Rect<Scalar, N>& other) const {
  for (int i = 0; i < N; ++i) {
    if (!rect_internal::PartialOverlap(*this, other, i))
      return false;
  }
  return true;
}

template <typename Scalar, int N>
bool Rect<Scalar, N>::Touches(const Rect<Scalar, N>& other) const {
  // Rects that touch are *touching only* in at least one dimension and
  // *touching or overlapping* in *all* N dimensions.
  int touch_only_count = 0;
  int touch_or_overlap_count = 0;
  for (int i = 0; i < N; ++i) {
    bool touch = rect_internal::PartialTouch(*this, other, i);
    bool overlap = rect_internal::PartialOverlap(*this, other, i);
    touch_only_count += (touch && !overlap);
    touch_or_overlap_count += (touch || overlap);
  }
  return (touch_only_count > 0) && (touch_or_overlap_count == N);
}

template <typename Scalar, int N>
int Rect<Scalar, N>::GetTouchingDimension(const Rect<Scalar, N>& other) const {
  for (int i = 0; i < N; ++i) {
    if (rect_internal::PartialTouch(*this, other, i))
      return i;
  }
  return -1;
}

template <typename Scalar, int N>
bool Rect<Scalar, N>::Contains(const Point<Scalar, N>& point) const {
  for (int i = 0; i < N; ++i) {
    if (point[i] < pos[i] || point[i] >= (pos[i] + size[i]))
      return false;
  }
  return true;
}

template <typename Scalar, int N>
bool Rect<Scalar, N>::Contains(const Rect<Scalar, N>& other) const {
  return Contains(other.pos) && Contains(other.pos + other.size - 1l);
}

template <typename Scalar, int N>
Rect<Scalar, N> Rect<Scalar, N>::GetOverlap(
    const Rect<Scalar, N>& other) const {
  Rect<Scalar, N> result;
  for (int i = 0; i < N; ++i) {
    rect_internal::ComputePartialOverlap(*this, other, i, &result);
  }
  return result;
}

}  // namespace engine2

#endif  // ENGINE2_RECT_H_