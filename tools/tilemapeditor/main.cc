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

  constexpr int kWindowFlags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
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

  window->Maximize();
  Vec<int64_t, 2> logical_size = graphics->GetSize().size / 4l;
  std::cerr << "logical size: " << logical_size.x() << " " << logical_size.y()
            << "\n";
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
              /*position_in_world=*/Point<>{});

  map.AddTileStacks({
      TileMap::TileStack{{TileMap::Tile{&white_sprite}}},
      TileMap::TileStack{{TileMap::Tile{&black_sprite}}},
      TileMap::TileStack{{TileMap::Tile{&lines_sprite}}},
      TileMap::TileStack{{
          TileMap::Tile{&lines_sprite},
          TileMap::Tile{&scribble_sprite},
      }},
  });

  // Set grid randomly
  Point<> point;
  for (point.y() = 0; point.y() < 50; ++point.y()) {
    for (point.x() = 0; point.x() < 50; ++point.x())
      map.SetTileStackAtGridPosition(point, rand() % 4);
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