#ifndef ENGINE2_TYPES_H_
#define ENGINE2_TYPES_H_

#include <functional>

#include <SDL2/SDL_events.h>

namespace engine2 {

using Callback = std::function<void()>;

// Event callbacks
using QuitCallback = std::function<void(const SDL_QuitEvent&)>;
using KeyboardCallback = std::function<void(const SDL_KeyboardEvent&)>;
using MouseButtonCallback = std::function<void(const SDL_MouseButtonEvent&)>;
using MouseWheelCallback = std::function<void(const SDL_MouseWheelEvent&)>;
using MouseMotionCallback = std::function<void(const SDL_MouseMotionEvent&)>;

enum class MouseButton {
  kLeft = SDL_BUTTON_LEFT,
  kMiddle = SDL_BUTTON_MIDDLE,
  kRight = SDL_BUTTON_RIGHT,
  kX1 = SDL_BUTTON_X1,
  kX2 = SDL_BUTTON_X2,
};

}  // namespace engine2

#endif  // ENGINE2_TYPES_H_