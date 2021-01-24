#include "engine2/window.h"

namespace engine2 {

// static
std::unique_ptr<Window> Window::Create(const std::string& title,
                                       const Rect<int, 2>& rect,
                                       const SDLWindowFlags& flags) {
  SDL_Window* sdl_window = SDL_CreateWindow(
      title.c_str(), rect.x(), rect.y(), rect.w(), rect.h(), flags.to_ulong());
  if (!sdl_window)
    return nullptr;
  return std::make_unique<Window>(sdl_window);
}

Window::Window(SDL_Window* sdl_window) : window_(sdl_window) {}

Window::~Window() {
  if (window_)
    SDL_DestroyWindow(window_);
}

Vec<int, 2> Window::GetSize() const {
  int x, y;
  SDL_GetWindowSize(window_, &x, &y);
  return {x, y};
}

Vec<int, 2> Window::GetMaximumSize() const {
  int x, y;
  SDL_GetWindowMaximumSize(window_, &x, &y);
  return {x, y};
}

Vec<int, 2> Window::GetMinimumSize() const {
  int x, y;
  SDL_GetWindowMinimumSize(window_, &x, &y);
  return {x, y};
}

void Window::Maximize() {
  SDL_MaximizeWindow(window_);
}

}  // namespace engine2