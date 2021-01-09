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
  auto window =
      Window::Create("tilemap editor", {0, 0, 1600, 1200}, /*flags=*/0);
  if (!window)
    return PrintSDLError("failed to create window");

  auto graphics = BasicGraphics2D::Create(*window, /*flags=*/0);
  if (!graphics)
    return PrintSDLError("failed to create renderer");

  graphics->SetLogicalSize(400, 300);

  // TODO try to load example tile images here
  auto texture =
      Texture::LoadFromImage(*graphics, "tools/tilemapeditor/tiles.png");
  if (!texture)
    return PrintSDLError("failed to load image");

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

  tilemapeditor::Editor(std::move(graphics), &map).Run();
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