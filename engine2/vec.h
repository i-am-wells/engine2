#ifndef ENGINE2_VEC_H_
#define ENGINE2_VEC_H_

#include <initializer_list>

namespace engine2 {

template <typename Scalar, int N>
struct Vec {
  Scalar value[N];

  // TODO static asserts
  Scalar& x() { return value[0]; }
  const Scalar& x() const { return value[0]; }
  Scalar& y() { return value[1]; }
  const Scalar& y() const { return value[1]; }
  Scalar& z() { return value[2]; }
  const Scalar& z() const { return value[2]; }
  Scalar& a() { return value[3]; }
  const Scalar& a() const { return value[3]; }

  static Vec Ones() {
    Vec result;
    for (int i = 0; i < N; ++i)
      result[i] = 1;
    return result;
  }

  template <typename OtherScalar>
  Vec<OtherScalar, N> ConvertTo() const {
    Vec<OtherScalar, N> result;
    for (int i = 0; i < N; ++i)
      result[i] = value[i];
    return result;
  }

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

}  // namespace engine2

#endif  // ENGINE2_VEC_H_