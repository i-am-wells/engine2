#include <cstdlib>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "engine2/impl/basic_graphics2d.h"
#include "engine2/sprite.h"
#include "engine2/texture.h"
#include "engine2/tile_map.h"
#include "engine2/vec.h"
#include "engine2/video_context.h"
#include "engine2/window.h"

#include "tools/tilemapeditor/editor.h"

using namespace engine2;

namespace {

void PrintSDLError(const std::string& message) {
  std::cerr << message << ": " << SDL_GetError() << "\n";
}

void LoadMapAndRun() {
  SDL_Rect display_bounds;
  if (SDL_GetDisplayUsableBounds(0, &display_bounds) != 0)
    return PrintSDLError("failed to get display bounds");

  constexpr int kWindowFlags =
      SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE;
  auto window = Window::Create("tilemap editor",
                               {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                display_bounds.w, display_bounds.h},
                               kWindowFlags);
  if (!window)
    return PrintSDLError("failed to create window");

  constexpr int kRenderFlags = SDL_RENDERER_ACCELERATED;
  auto graphics = BasicGraphics2D::Create(*window, kRenderFlags);
  if (!graphics)
    return PrintSDLError("failed to create renderer");

  Vec<int64_t, 2> logical_size = graphics->GetSize().size / 4l;
  graphics->SetLogicalSize(logical_size.x(), logical_size.y());

  // TODO try to load example tile images here
  auto texture =
      Texture::LoadFromImage(*graphics, "tools/tilemapeditor/tiles.png");
  if (!texture)
    return PrintSDLError("failed to load image");

  auto font = Font::Load("tools/tilemapeditor/ter-u12b.otb", 12);
  if (!font)
    return PrintSDLError("failed to load font");

  // Create sprites
  Sprite white_sprite(texture.get(), {0, 0, 16, 16});
  Sprite lines_sprite(texture.get(), {16, 0, 16, 16});
  Sprite black_sprite(texture.get(), {16, 16, 16, 16});
  Sprite scribble_sprite(texture.get(), {0, 16, 16, 16});

  // Create a tile map
  TileMap map(/*tile_size=*/Vec<int, 2>{16, 16},
              /*grid_size=*/Vec<int64_t, 2>{50, 50},
              /*layer_count=*/2,
              /*position_in_world=*/Point<>{});

  map.AddTiles({
      {nullptr},
      {&white_sprite},
      {&lines_sprite},
      {&black_sprite},
      {&scribble_sprite},
  });

  // Set grid randomly
  TileMap::GridPoint point;
  for (point.y() = 0; point.y() < 50; ++point.y()) {
    for (point.x() = 0; point.x() < 50; ++point.x()) {
      map.SetTileIndex(point, /*layer=*/0, (rand() % 4) + 1);
      map.SetTileIndex(point, 1, 0);
    }
  }

  tilemapeditor::Editor(window.get(), graphics.get(), font.get(), &map).Run();
}

}  // namespace

int main(int argc, char** argv) {
  if (!(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0) ||
      !(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG) || !(TTF_Init() == 0)) {
    PrintSDLError("failed to init SDL");
    return 1;
  }

  LoadMapAndRun();

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return 0;
}