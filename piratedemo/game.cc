#include <fstream>
#include <iostream>

#include "engine2/rgba_color.h"
#include "piratedemo/data.h"
#include "piratedemo/game.h"

using namespace engine2;

namespace piratedemo {

constexpr Rect<> kWorldRect{-500, -500, 1000, 1000};

Game::Game(Window* window, Graphics2D* graphics)
    : FrameLoop(/*event_handler=*/this),
      window_(window),
      graphics_(graphics),
      texture_cache_(graphics),
      sprite_cache_(&texture_cache_),
      player_(&sprite_cache_),
      camera_({}, {}) {
  Vec<int64_t, 2> window_size = graphics->GetSize().size;
  Point<> camera_pos = -window_size;
  camera_pos /= 2;
  camera_.SetWorldRect({camera_pos, window_size});
}

bool Game::Load() {
  if (!sprite_cache_.LoadSpriteSheet(MakeShipSpriteSheet()) ||
      !sprite_cache_.LoadSpriteSheet(MakePlayerSpriteSheet())) {
    std::cerr << "Failed to load sprites\n";
    return false;
  }

  const std::string map_path = "piratedemo/ship.map";
  std::ifstream map_file(map_path, std::ios::binary);
  if (map_file.fail()) {
    std::cerr << "Can't open " << map_path << ".\n";
    return false;
  }

  map_ = TileMap::Read(map_file, &sprite_cache_);
  if (!map_) {
    std::cerr << "File " << map_path
              << " exists but isn't a valid tile map file.\n";
    return false;
  }

  // TODO Create walls from ship tile map
}

void Game::EveryFrame() {
  graphics_->SetDrawColor(kBlack)->Clear();

  // Draw map
  // Draw objects?

  graphics_->Present();

  // Update
}

}  // namespace piratedemo