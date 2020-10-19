#ifndef ENGINE2_POINT_H_
#define ENGINE2_POINT_H_

#include <cstdint>

#include "engine2/vec.h"

namespace engine2 {

template <typename Scalar = int64_t, int N = 2>
using Point = Vec<Scalar, N>;

}  // namespace engine2

#endif  // ENGINE2_POINT_H_