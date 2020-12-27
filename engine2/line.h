#ifndef ENGINE2_LINE_H_
#define ENGINE2_LINE_H_

#include "engine2/point.h"

namespace engine2 {

template <int N, class Scalar>
struct Line {
  Point<Scalar, N> a, b;
};

}  // namespace engine2

#endif  // ENGINE2_LINE_H_