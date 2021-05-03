#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "engine2/impl/basic_graphics2d.h"
#include "engine2/window.h"

#include "piratedemo/game.h"

namespace {

const bool kShowTitleScreen = true;

int PrintSDLError(const std::string& msg) {
  std::cerr << msg << ": " << SDL_GetError() << '\n';
  return 1;
}

int Run() {
  auto window = engine2::Window::Create(
      "piratedemo", {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600},
      SDL_WINDOW_ALLOW_HIGHDPI);
  if (!window)
    return PrintSDLError("Couldn't create window");

  auto graphics =
      engine2::BasicGraphics2D::Create(*window, SDL_RENDERER_ACCELERATED);
  if (!graphics)
    return PrintSDLError("Couldn't create renderer");
  graphics->SetLogicalSize(400, 300);

  auto font = engine2::Font::Load("piratedemo/ter-u12b.otb", 12);
  if (!font)
    return PrintSDLError("Couldn't load font");

  piratedemo::Game game(window.get(), graphics.get(), font.get());
  if (!game.Load())
    return 1;
  game.Run();

  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  if (!(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0) ||
      !(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG) || !(TTF_Init() == 0)) {
    return PrintSDLError("failed to init SDL");
  }
  int ret = Run();

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return ret;
}