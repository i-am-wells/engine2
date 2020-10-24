#ifndef ENGINE2_RECT_OBJECT_H_
#define ENGINE2_RECT_OBJECT_H_

#include "engine2/rect.h"

namespace engine2 {

template <typename T, int N>
class RectObject {
 public:
  virtual Rect<T, N> GetRect() = 0;
  virtual ~RectObject() = default;
};

}  // namespace engine2

#endif  // ENGINE2_RECT_OBJECT_H_