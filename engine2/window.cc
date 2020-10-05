#include "engine2/window.h"

namespace engine2 {

// static
std::unique_ptr<Window> Window::Create(const std::string& title,
                                       const Rect& rect,
                                       const SDLWindowFlags& flags) {
  SDL_Window* sdl_window = SDL_CreateWindow(title.c_str(), rect.x, rect.y,
                                            rect.w, rect.h, flags.to_ulong());
  if (!sdl_window)
    return nullptr;
  return std::make_unique<Window>(sdl_window);
}

Window::Window(SDL_Window* sdl_window) : window_(sdl_window) {}

Window::~Window() {
  if (window_)
    SDL_DestroyWindow(window_);
}

}  // namespace engine2