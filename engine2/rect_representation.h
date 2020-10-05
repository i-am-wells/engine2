#ifndef ENGINE2_RECT_REPRESENTATION_H_
#define ENGINE2_RECT_REPRESENTATION_H_

#include "engine2/rect.h"

namespace engine2 {

class RectRepresentation {
 public:
  enum InteractionType {
    kVideo,
    kAudio,
    kPhysics,
  };

  virtual Rect GetRect() = 0;

  virtual void OnTouch(const RectRepresentation* other) = 0;
  virtual void OnOverlap(const RectRepresentation* other) = 0;
};

}  // namespace engine2

#endif  // ENGINE2_RECT_REPRESENTATION_H_