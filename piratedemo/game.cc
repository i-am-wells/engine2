#include <fstream>
#include <iostream>

#include "engine2/performance/perf_span.h"
#include "engine2/performance/scoped_stopwatch.h"
#include "engine2/rgba_color.h"
#include "engine2/tile_map.h"
#include "piratedemo/data.h"
#include "piratedemo/game.h"
#include "piratedemo/types.h"

using namespace engine2;

namespace piratedemo {
namespace {

constexpr Rect<> kWorldRect{-160, -160, 320, 320};
const int kPlayerMoveVelocity = 64;  // pixels per second

Vec<double, 2> VelocityForDirection(Direction direction) {
  switch (direction) {
    case Direction::kNorth:
      return {0, -kPlayerMoveVelocity};
    case Direction::kSouth:
      return {0, kPlayerMoveVelocity};
    case Direction::kEast:
      return {kPlayerMoveVelocity, 0};
    case Direction::kWest:
      return {-kPlayerMoveVelocity, 0};
  }
}

}  // namespace

Game::Game(Window* window, Graphics2D* graphics, Font* font)
    : FrameLoop(/*event_handler=*/this),
      window_(window),
      graphics_(graphics),
      texture_cache_(graphics),
      sprite_cache_(&texture_cache_),
      player_(this, /*start_point=*/{0, 0}, graphics, &camera_),
      camera_({}, {}),
      space_(kWorldRect) {
  Vec<int64_t, 2> window_size = graphics->GetLogicalSize().size;
  Point<> camera_pos = -window_size;
  camera_pos /= 2;
  camera_.SetWorldRect({camera_pos, window_size});
  camera_.SetWindowRect(graphics->GetLogicalSize());

  space_.Add(&player_);

#ifdef PERF
  frame_stats_overlay_ = std::make_unique<FrameStatsOverlay>(
      graphics, font,
      std::vector<PerfSpan::Id>{PerfSpan::Id::kFrame,
                                PerfSpan::Id::kFrameIdle});
#endif
}

bool Game::Load() {
  if (!sprite_cache_.LoadSpriteSheet(MakeShipSpriteSheet())) {
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

  if (!Player::Load(graphics_)) {
    std::cerr << "Failed to load player resources.\n";
    return false;
  }

  // Create walls
  uint32_t wall_tag_id = map_->GetTagId("wall");
  if (wall_tag_id == TileMap::kTagNotFound) {
    std::cerr << "Tile map doesn't have walls\n";
    return false;
  }

  TileMap::GridPoint p;
  auto map_size = map_->GetGridSize();
  for (p.y() = 0; p.y() < map_size.y(); ++p.y()) {
    for (p.x() = 0; p.x() < map_size.x(); ++p.x()) {
      TileMap::Tile* tile = map_->GetTile(p, 0);
      if (tile && tile->HasTag(wall_tag_id)) {
        walls_.emplace_back(this,
                            Rect<>{map_->GridToWorld(p), map_->GetTileSize()});
        space_.Add(&walls_.back());
      }
    }
  }

  return true;
}

void Game::EveryFrame() {
  TIME_THIS_SCOPE_AS(PerfSpan::Id::kFrame);

  graphics_->SetDrawColor(kWhite)->Clear();

  map_->Draw(graphics_, camera_.GetRect(), camera_.GetWindowRect());

  // TODO this probably belongs in camera2d.h
  for (auto& variant : space_.Near(camera_.GetRect())) {
    std::visit(
        [this](auto* object) {
          if (camera_.GetRect().Overlaps(object->GetRect()))
            camera_.OnOverlap(object);
        },
        variant);
  }
  camera_.Draw();

#ifdef PERF
  frame_stats_overlay_->Draw();
  Time perf_now = Time::Now();
  if (perf_now > last_perf_flush_ + perf_flush_period_) {
    gPerfRecorder.Flush();
    last_perf_flush_ = perf_now;
  }
#endif

  graphics_->Present();

  space_.AdvanceTime(Time::Now() - last_update_time_);
  last_update_time_ = Time::Now();

  {
    TIME_THIS_SCOPE_AS(PerfSpan::Id::kFrameIdle);
    idler_.Wait();
  }
}

void Game::OnKeyDown(const SDL_KeyboardEvent& event) {
  if (event.repeat)
    return;

  switch (event.keysym.sym) {
    case SDLK_w:
      MovePlayer(Direction::kNorth, true);
      break;
    case SDLK_a:
      MovePlayer(Direction::kWest, true);
      break;
    case SDLK_s:
      MovePlayer(Direction::kSouth, true);
      break;
    case SDLK_d:
      MovePlayer(Direction::kEast, true);
      break;
    default:
      break;
  }
}

void Game::OnKeyUp(const SDL_KeyboardEvent& event) {
  if (event.repeat)
    return;

  switch (event.keysym.sym) {
    case SDLK_w:
      MovePlayer(Direction::kNorth, false);
      break;
    case SDLK_a:
      MovePlayer(Direction::kWest, false);
      break;
    case SDLK_s:
      MovePlayer(Direction::kSouth, false);
      break;
    case SDLK_d:
      MovePlayer(Direction::kEast, false);
      break;
    default:
      break;
  }
}

void Game::MovePlayer(Direction direction, bool key_down) {
  if (key_down) {
    move_keypress_stack_.push_back(direction);
    player_.Face(direction);
    player_.velocity() += VelocityForDirection(direction);
  } else {
    // Remove direction from stack
    for (auto iter = move_keypress_stack_.begin();
         iter != move_keypress_stack_.end(); ++iter) {
      if (*iter == direction) {
        move_keypress_stack_.erase(iter);
        break;
      }
    }

    player_.velocity() -= VelocityForDirection(direction);
    if (!move_keypress_stack_.empty())
      player_.Face(move_keypress_stack_.back());
  }

  if (player_.velocity().IsZero())
    player_.SetMovement(Player::Movement::kStand);
  else
    player_.SetMovement(Player::Movement::kWalk);
}

}  // namespace piratedemo