#include "point.h"

namespace engine2 {

int64_t& Point::operator[](int i) {
  return p[i];
}

const int64_t& Point::operator[](int i) const {
  return p[i];
}

bool Point::operator==(const Point& other) const {
  return x == other.x && y == other.y;
}

Point& Point::operator+=(const Point& other) {
  x += other.x;
  y += other.y;
  return *this;
}

Point operator+(const Point& a, const Point& b) {
  return Point(a) += b;
}

Point& Point::operator-=(const Point& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

Point operator-(const Point& a, const Point& b) {
  return Point(a) -= b;
}

}  // namespace engine2