#ifndef ENGINE2_POINT_H_
#define ENGINE2_POINT_H_

#include <cstdint>

namespace engine2 {

struct Point {
  union {
    int64_t p[2];
    struct {
      int64_t x, y;
    };
  };

  int64_t& operator[](int i);
  const int64_t& operator[](int i) const;

  bool operator==(const Point& other) const;
  Point& operator+=(const Point& other);
  Point& operator-=(const Point& other);
};

Point operator+(const Point& a, const Point& b);
Point operator-(const Point& a, const Point& b);

}  // namespace engine2

#endif  // ENGINE2_POINT_H_