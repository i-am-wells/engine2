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

Vec<int, 2> Window::GetDisplaySize() const {
  int display_index = SDL_GetWindowDisplayIndex(window_);
  if (display_index == -1)
    return {};

  SDL_Rect display_rect;
  if (SDL_GetDisplayBounds(display_index, &display_rect) != 0)
    return {};

  return {display_rect.w, display_rect.h};
}

void Window::Maximize() {
  SDL_MaximizeWindow(window_);
}

Point<int, 2> Window::GetInnerPosition() const {
  int pos_x, pos_y;
  SDL_GetWindowPosition(window_, &pos_x, &pos_y);

  int border_x = 0;
  int border_y = 0;
  SDL_GetWindowBordersSize(window_, &border_y, &border_x, nullptr, nullptr);

  return {pos_x + border_x, pos_y + border_y};
}

}  // namespace engine2