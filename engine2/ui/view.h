#ifndef ENGINE2_UI_VIEW_H_
#define ENGINE2_UI_VIEW_H_

#include <SDL2/SDL_events.h>

#include "engine2/rect.h"

namespace engine2 {
namespace ui {

/**
 * Layout of a view:
 *
 * + - - - - - - - - - - - - - - - - - - - - - +
 * | 1. margin ^                               |
 *     +-----------------------------------+
 * |   |  2. padding v                     |   |
 *     |   + - - - - - - - - - - - - - +   |
 * |   |   |                           |   |   |
 *     |      content or child views       |
 * |   |   |                           |   |   |
 *     |   + - - - - - - - - - - - - - +   |
 * |   |                                   |   |
 *     +---------3.- GetRect()-------------+
 * |                                           |
 * + - - - - - - - - - - - - - - - - - - - - - +
 *
 * 1. Margin is extra space considered to be _outside_ the view.
 * 2. Padding is extra space considered to be _inside_ the view.
 * 3. When calculating size, GetRect() adds padding to the size of the view's
 * contents.
 */
class View {
 public:
  // Relative position: added to parent's absolute position to calculate own
  // absolute position.
  virtual Point<int, 2> GetRelativePosition() const = 0;
  virtual void SetRelativePosition(const Point<int, 2>& pos) = 0;

  virtual Point<int, 2> GetAbsolutePosition() const = 0;
  virtual Point<int, 2> GetAbsoluteInnerPosition() const = 0;

  virtual Vec<int, 2> GetSize() const = 0;
  virtual void SetSize(const Vec<int, 2>& size) = 0;

  // Get the absolute position and size.
  virtual Rect<int, 2> GetRect() const = 0;

  // Draw to the window.
  virtual void Draw() const {};

  // Should be called when this view's size changes.
  virtual void OnSizeChanged() = 0;

  virtual Vec<int, 2> GetPadding() const { return {0, 0}; };
  virtual Vec<int, 2> GetMargin() const { return {0, 0}; }

  // Handle mouse events.
  virtual void OnMouseButtonDown(const SDL_MouseButtonEvent& event) {}
  virtual void OnMouseButtonUp(const SDL_MouseButtonEvent& event) {}
  virtual void OnMouseMotion(const SDL_MouseMotionEvent& event) {}
  virtual void OnMouseWheel(const SDL_MouseWheelEvent& event) {}
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_VIEW_H_