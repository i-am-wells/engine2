#include <cstdlib>
#include <fstream>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "engine2/command_line_parser.h"
#include "engine2/impl/basic_graphics2d.h"
#include "engine2/sprite.h"
#include "engine2/sprite_cache.h"
#include "engine2/texture.h"
#include "engine2/texture_cache.h"
#include "engine2/tile_map.h"
#include "engine2/vec.h"
#include "engine2/video_context.h"
#include "engine2/window.h"

#include "tools/tilemapeditor/editor.h"

using namespace engine2;

namespace {

enum class Mode { kCreate, kEdit };

void PrintSDLError(const std::string& message) {
  std::cerr << message << ": " << SDL_GetError() << "\n";
}

void PrintUsage() {
  std::cerr << "tilemapeditor: Tool for creating and editing tile maps.\n\n"
            << "To edit an existing tile map: tilemapeditor edit "
               "map_file_name.map\n\n"
            << "To create a new tile map: tilemapeditor create \\\n"
            << "                              --sprite_sheet=sprites.lua \\\n"
            << "                              --tile_size=W,H \\\n"
            << "                              --layers=N \\\n"
            << "                              --grid_size=W,H \\\n"
            << "                              map_file_name.map\n\n";
}

std::pair<bool, engine2::Vec<int64_t, 2>> ParseSize(const std::string& str) {
  size_t pos = str.find(',');
  if (pos == std::string::npos)
    return {false, {}};

  std::string first = str.substr(0, pos);
  std::string second = str.substr(pos + 1);

  try {
    return {true,
            {std::stoi(str.substr(0, pos)), std::stoi(str.substr(pos + 1))}};
  } catch (const std::invalid_argument& e) {
    return {false, {}};
  }
}

std::pair<std::unique_ptr<engine2::Window>,
          std::unique_ptr<engine2::Graphics2D>>
CreateWindowAndRenderer() {
  SDL_Rect display_bounds;
  if (SDL_GetDisplayUsableBounds(0, &display_bounds) != 0) {
    PrintSDLError("failed to get display bounds");
    return {};
  }

  constexpr int kWindowFlags =
      SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP;
  auto window = Window::Create("tilemap editor",
                               {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                display_bounds.x, display_bounds.y},
                               kWindowFlags);
  if (!window)
    return {};

  constexpr int kRenderFlags = SDL_RENDERER_ACCELERATED;
  auto graphics = BasicGraphics2D::Create(*window, kRenderFlags);
  if (!graphics)
    return {};

  return {std::move(window), std::move(graphics)};
}

std::unique_ptr<engine2::TileMap> CreateMap(
    const engine2::CommandLineParser& flags,
    engine2::SpriteCache* sprite_cache) {
  engine2::Vec<int64_t, 2> tile_size;
  auto parsed_size = ParseSize(flags.GetFlag("tile_size"));
  if (parsed_size.first) {
    tile_size = parsed_size.second;
  } else {
    std::cerr << "Expected tile_size flag to look like this: --tile_size=W,H\n";
    return nullptr;
  }

  int layers;
  try {
    layers = std::stoi(flags.GetFlag("layers"));
  } catch (const std::invalid_argument& e) {
    std::cerr << "Expected layers flag to be an integer.\n";
    return nullptr;
  }

  engine2::Vec<int64_t, 2> grid_size;
  parsed_size = ParseSize(flags.GetFlag("grid_size"));
  if (parsed_size.first) {
    grid_size = parsed_size.second;
  } else {
    std::cerr << "Expected grid_size flag to look like this: --grid_size=W,H\n";
    return nullptr;
  }

  engine2::Vec<int64_t, 2> position_in_world;
  parsed_size = ParseSize(flags.GetFlag("grid_size"));
  if (parsed_size.first) {
    position_in_world = parsed_size.second;
  } else {
    std::cerr << "Expected position_in_world flag to look like this: "
                 "--position_in_world=X,Y\n";
    return nullptr;
  }

  auto map = std::make_unique<TileMap>(tile_size, grid_size, layers,
                                       position_in_world, sprite_cache);
  if (!map)
    return nullptr;
  map->AddTile({nullptr});

  return map;
}

std::unique_ptr<TileMap> LoadMap(const engine2::CommandLineParser& flags,
                                 const std::string& map_file_name,
                                 engine2::SpriteCache* sprite_cache) {
  std::ifstream map_file(map_file_name, std::ios::binary);
  if (map_file.fail()) {
    std::cerr << "Can't open " << map_file_name << ".\n";
    return nullptr;
  }

  return TileMap::Read(map_file, sprite_cache);
}

void Run(const engine2::CommandLineParser& flags) {
  Mode mode;
  if (flags.GetPositional(0) == "create") {
    mode = Mode::kCreate;
    if (!flags.CheckFlags({
            "sprite_sheet",
            "tile_size",
            "layers",
            "grid_size",
        })) {
      return PrintUsage();
    }
  } else if (flags.GetPositional(0) == "edit") {
    mode = Mode::kEdit;
  } else {
    std::cerr << "Missing 'create' or 'edit'.\n";
    return PrintUsage();
  }

  auto [window, graphics] = CreateWindowAndRenderer();
  if (!window)
    return PrintSDLError("failed to create window");

  if (!graphics)
    return PrintSDLError("failed to create renderer");

  auto icons_texture =
      Texture::LoadFromImage(*graphics, "tools/tilemapeditor/editor-icons.png");
  if (!icons_texture)
    return PrintSDLError("failed to load editor icons");

  auto font = Font::Load("tools/tilemapeditor/ter-u12b.otb", 12);
  if (!font)
    return PrintSDLError("failed to load font");

  TextureCache texture_cache(graphics.get());
  SpriteCache sprite_cache(&texture_cache);

  std::string map_file_name = flags.GetPositional(1);

  std::string initial_status_text;
  std::unique_ptr<TileMap> map;
  if (mode == Mode::kCreate) {
    map = CreateMap(flags, &sprite_cache);
    if (!map) {
      std::cerr << "Failed to create map.\n";
      return;
    }

    initial_status_text = "Created a new map to be saved as " + map_file_name;

    // TODO pass this in?
    sprite_cache.Get("tools/tilemapeditor/sprites.lua");
  } else if (mode == Mode::kEdit) {
    map = LoadMap(flags, map_file_name, &sprite_cache);
    if (!map) {
      std::cerr << "Failed to load map from " << map_file_name << ".\n";
      return;
    }

    initial_status_text = "Opened " + map_file_name;
  }

  tilemapeditor::Editor editor(window.get(), graphics.get(), font.get(),
                               map.get(), icons_texture.get(), &sprite_cache,
                               map_file_name, initial_status_text);
  editor.Init();
  editor.Run();
}

}  // namespace

int main(int argc, char** argv) {
  engine2::CommandLineParser command_line_parser(argc, argv);
  command_line_parser.Parse();

  if (!(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0) ||
      !(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG) || !(TTF_Init() == 0)) {
    PrintSDLError("failed to init SDL");
    return 1;
  }

  Run(command_line_parser);

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return 0;
}