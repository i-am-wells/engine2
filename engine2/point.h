#ifndef ENGINE2_POINT_H_
#define ENGINE2_POINT_H_

#include <cstdint>

namespace engine2 {

template <typename T = int64_t>
struct Point {
  union {
    T p[2];
    struct {
      T x, y;
    };
  };

  T& operator[](int i);
  const T& operator[](int i) const;

  bool operator==(const Point& other) const;
  Point& operator+=(const Point& other);
  Point& operator-=(const Point& other);
};

template <typename T>
Point<T> operator+(const Point<T>& a, const Point<T>& b);

template <typename T>
Point<T> operator-(const Point<T>& a, const Point<T>& b);

}  // namespace engine2

#include "engine2/point.cc"

#endif  // ENGINE2_POINT_H_