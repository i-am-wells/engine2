#ifndef ENGINE2_UI_VIEW_H_
#define ENGINE2_UI_VIEW_H_

#include <SDL2/SDL_events.h>

#include "engine2/rect.h"

namespace engine2 {
namespace ui {

class View {
 public:
  virtual void SetPosition(const Point<int, 2>& position) = 0;
  virtual Rect<int, 2> GetRect() const = 0;

  virtual void Draw() const = 0;

  virtual void OnMouseDown(const SDL_MouseButtonEvent& event) {}
  virtual void OnMouseUp(const SDL_MouseButtonEvent& event) {}
  virtual void OnMouseMotion(const SDL_MouseMotionEvent& event) {}
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_VIEW_H_