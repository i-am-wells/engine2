#ifndef ENGINE2_RECT_OBJECT_H_
#define ENGINE2_RECT_OBJECT_H_

#include "engine2/rect.h"

namespace engine2 {

class RectObject {
 public:
  virtual Rect GetRect() = 0;
  virtual void OnOverlap(RectObject* other) {}
  virtual void OnTouch(RectObject* other) {}
};

}  // namespace engine2

#endif  // ENGINE2_RECT_OBJECT_H_