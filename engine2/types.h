#ifndef ENGINE2_TYPES_H_
#define ENGINE2_TYPES_H_

#include <functional>

#include <SDL2/SDL_events.h>

namespace engine2 {

using Callback = std::function<void()>;

// Event callbacks
using QuitCallback = std::function<void(const SDL_QuitEvent&)>;
using KeyboardCallback = std::function<void(const SDL_KeyboardEvent&)>;

}  // namespace engine2

#endif  // ENGINE2_TYPES_H_