#ifndef TOOLS_TILEMAPEDITOR_TWO_FINGER_TOUCH_H_
#define TOOLS_TILEMAPEDITOR_TWO_FINGER_TOUCH_H_

#include <map>

#include <SDL2/SDL_events.h>

#include "engine2/point.h"
#include "engine2/vec.h"

namespace tilemapeditor {

class TwoFingerTouch {
 public:
  class Handler {
   public:
    virtual void OnPinch(const engine2::Point<double, 2>& center,
                         double pinch_factor) = 0;
    virtual void OnDrag(const engine2::Vec<double, 2>& drag_amount) = 0;
  };

  TwoFingerTouch(Handler* handler);

  void OnFingerDown(const SDL_TouchFingerEvent& event);
  void OnFingerUp(const SDL_TouchFingerEvent& event);
  void OnFingerMotion(const SDL_TouchFingerEvent& event);

 private:
  void UpdateCenter();

  Handler* handler_;
  // Map finger IDs to touch points.
  std::map<int, engine2::Point<double, 2>> points_;
  engine2::Point<double, 2> center_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_TWO_FINGER_TOUCH_H_