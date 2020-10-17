#ifndef ENGINE2_VEC_H_
#define ENGINE2_VEC_H_

namespace engine2 {

template <typename Scalar, int N>
struct Vec {
  Scalar value[N];

  Scalar& operator[](int i) { return value[i]; }
  const Scalar& operator[](int i) const { return value[i]; }

  bool operator==(const Vec& other) const {
    for (int i = 0; i < N; ++i) {
      if (value[i] != other[i])
        return false;
    }
    return true;
  }

  Vec operator-() const {
    Vec result;
    for (int i = 0; i < N; ++i)
      result[i] = -value[i];
    return result;
  }

  Vec& operator+=(const Vec& other) {
    for (int i = 0; i < N; ++i) {
      value[i] += other[i];
    };
    return *this;
  }

  Vec& operator-=(const Vec& other) { return *this += -other; }

  Vec& operator*=(const Vec& other) {
    for (int i = 0; i < N; ++i) {
      value[i] *= other[i];
    };
    return *this;
  }

  Vec& operator/=(const Vec& other) {
    for (int i = 0; i < N; ++i) {
      value[i] /= other[i];
    };
    return *this;
  }

  Vec& operator+=(Scalar scalar) {
    for (int i = 0; i < N; ++i) {
      value[i] += scalar;
    };
    return *this;
  }

  Vec& operator-=(Scalar scalar) {
    for (int i = 0; i < N; ++i) {
      value[i] -= scalar;
    };
    return *this;
  }

  Vec& operator*=(Scalar scalar) {
    for (int i = 0; i < N; ++i) {
      value[i] *= scalar;
    };
    return *this;
  }

  Vec& operator/=(Scalar scalar) {
    for (int i = 0; i < N; ++i) {
      value[i] /= scalar;
    };
    return *this;
  }
};

template <typename Scalar, int N>
Vec<Scalar, N> operator+(const Vec<Scalar, N>& l, const Vec<Scalar, N>& r) {
  return Vec<Scalar, N>(l) += r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator-(const Vec<Scalar, N>& l, const Vec<Scalar, N>& r) {
  return Vec<Scalar, N>(l) -= r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator*(const Vec<Scalar, N>& l, const Vec<Scalar, N>& r) {
  return Vec<Scalar, N>(l) *= r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator/(const Vec<Scalar, N>& l, const Vec<Scalar, N>& r) {
  return Vec<Scalar, N>(l) /= r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator+(const Vec<Scalar, N>& l, Scalar r) {
  return Vec<Scalar, N>(l) += r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator-(const Vec<Scalar, N>& l, Scalar r) {
  return Vec<Scalar, N>(l) -= r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator*(const Vec<Scalar, N>& l, Scalar r) {
  return Vec<Scalar, N>(l) *= r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator/(const Vec<Scalar, N>& l, Scalar r) {
  return Vec<Scalar, N>(l) /= r;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator+(Scalar l, const Vec<Scalar, N>& r) {
  return Vec<Scalar, N>(r) += l;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator-(Scalar l, const Vec<Scalar, N>& r) {
  return l + (-r);
}

template <typename Scalar, int N>
Vec<Scalar, N> operator*(Scalar l, const Vec<Scalar, N>& r) {
  return Vec<Scalar, N>(r) *= l;
}

template <typename Scalar, int N>
Vec<Scalar, N> operator/(Scalar l, const Vec<Scalar, N>& r) {
  Vec<Scalar, N> result;
  for (int i = 0; i < N; ++i)
    result[i] = l / r[i];
  return result;
}

#define VEC_SPECIAL_BEGIN(name, dim) \
  template <typename Scalar>         \
  struct name {                      \
    union {                          \
      Vec<Scalar, dim> value;        \
      struct {
// Note: Named fields go here!
#define VEC_SPECIAL_END(name)                                            \
  }                                                                      \
  ;                                                                      \
  }                                                                      \
  ;                                                                      \
  bool operator==(const name& other) { return value == other.value; }    \
  name operator-() const { return -value; }                              \
  name& operator+=(const name& other) {                                  \
    value += other.value;                                                \
    return *this;                                                        \
  }                                                                      \
  name& operator-=(const name& other) {                                  \
    value -= other.value;                                                \
    return *this;                                                        \
  }                                                                      \
  name& operator*=(const name& other) {                                  \
    value *= other.value;                                                \
    return *this;                                                        \
  }                                                                      \
  name& operator/=(const name& other) {                                  \
    value /= other.value;                                                \
    return *this;                                                        \
  }                                                                      \
  name& operator+=(Scalar scalar) {                                      \
    value += scalar;                                                     \
    return *this;                                                        \
  }                                                                      \
  name& operator-=(Scalar scalar) {                                      \
    value -= scalar;                                                     \
    return *this;                                                        \
  }                                                                      \
  name& operator*=(Scalar scalar) {                                      \
    value *= scalar;                                                     \
    return *this;                                                        \
  }                                                                      \
  name& operator/=(Scalar scalar) {                                      \
    value /= scalar;                                                     \
    return *this;                                                        \
  }                                                                      \
  }                                                                      \
  ;                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator+(const name<Scalar>& l, const name<Scalar>& r) { \
    return {l.value + r.value};                                          \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator-(const name<Scalar>& l, const name<Scalar>& r) { \
    return {l.value - r.value};                                          \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator*(const name<Scalar>& l, const name<Scalar>& r) { \
    return {l.value * r.value};                                          \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator/(const name<Scalar>& l, const name<Scalar>& r) { \
    return {l.value / r.value};                                          \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator+(const name<Scalar>& l, Scalar r) {              \
    return {l.value + r};                                                \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator-(const name<Scalar>& l, Scalar r) {              \
    return {l.value - r};                                                \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator*(const name<Scalar>& l, Scalar r) {              \
    return {l.value * r};                                                \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator/(const name<Scalar>& l, Scalar r) {              \
    return {l.value / r};                                                \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator+(Scalar l, const name<Scalar>& r) {              \
    return {l + r.value};                                                \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator-(Scalar l, const name<Scalar>& r) {              \
    return {l - r.value};                                                \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator*(Scalar l, const name<Scalar>& r) {              \
    return {l * r.value};                                                \
  }                                                                      \
  template <typename Scalar>                                             \
  name<Scalar> operator/(Scalar l, const name<Scalar>& r) {              \
    return {l / r.value};                                                \
  }

// Define Vec2, Vec3, and Vec4
VEC_SPECIAL_BEGIN(Vec2, 2)
Scalar x, y;
VEC_SPECIAL_END(Vec2)

VEC_SPECIAL_BEGIN(Vec3, 3)
Scalar x, y, z;
VEC_SPECIAL_END(Vec3)

VEC_SPECIAL_BEGIN(Vec4, 4)
Scalar x, y, z, a;
VEC_SPECIAL_END(Vec4)

}  // namespace engine2

#endif  // ENGINE2_VEC_H_