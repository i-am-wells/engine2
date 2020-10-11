#ifndef ENGINE2_WINDOW_H_
#define ENGINE2_WINDOW_H_

#include <SDL2/SDL_video.h>

#include <bitset>
#include <memory>

#include "engine2/rect.h"

namespace engine2 {

class BasicGraphics2D;
using SDLWindowFlags = std::bitset<32>;

class Window {
 public:
  // Create a window. Returns a null unique_ptr if something went wrong.
  static std::unique_ptr<Window> Create(const std::string& title,
                                        const Rect<>& rect,
                                        const SDLWindowFlags& flags);
  // The destructor calls SDL_DestroyWindow(), so if you use this constructor,
  // don't destroy the window yourself.
  explicit Window(SDL_Window* sdl_window);
  ~Window();

 private:
  friend class BasicGraphics2D;
  SDL_Window* window_;
};

}  // namespace engine2

#endif  // ENGINE2_WINDOW_H_