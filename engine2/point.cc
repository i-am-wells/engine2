//#include "point.h"

namespace engine2 {

template <typename T>
T& Point<T>::operator[](int i) {
  return p[i];
}

template <typename T>
const T& Point<T>::operator[](int i) const {
  return p[i];
}

template <typename T>
bool Point<T>::operator==(const Point<T>& other) const {
  return x == other.x && y == other.y;
}

template <typename T>
Point<T>& Point<T>::operator+=(const Point<T>& other) {
  x += other.x;
  y += other.y;
  return *this;
}

template <typename T>
Point<T> operator+(const Point<T>& a, const Point<T>& b) {
  return Point<T>(a) += b;
}

template <typename T>
Point<T>& Point<T>::operator-=(const Point<T>& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

template <typename T>
Point<T> operator-(const Point<T>& a, const Point<T>& b) {
  return Point<T>(a) -= b;
}

template <typename T>
Point<T> operator*(const Point<T>& a, const Point<T>& b) {
  return {a.x * b.x, a.y * b.y};
}

template <typename T>
Point<T> operator*(const Point<T>& p, const T& scalar) {
  return p * Point<T>{scalar, scalar};
}

template <typename T>
Point<T> operator/(const Point<T>& a, const Point<T>& b) {
  return {a.x / b.x, a.y / b.y};
}

template <typename T>
Point<T> operator/(const Point<T>& p, const T& scalar) {
  return p / Point<T>{scalar, scalar};
}

}  // namespace engine2