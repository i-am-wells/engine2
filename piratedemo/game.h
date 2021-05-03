#ifndef PIRATEDEMO_GAME_H_
#define PIRATEDEMO_GAME_H_

#include <memory>

#include "engine2/camera2d.h"
#include "engine2/event_handler.h"
#include "engine2/font.h"
#include "engine2/frame_loop.h"
#include "engine2/space.h"
#include "engine2/tile_map.h"
#include "engine2/time.h"
#include "engine2/timing.h"

#ifdef PERF
#include "engine2/performance/frame_stats_overlay.h"
#include "engine2/performance/scoped_stopwatch.h"
#endif

#include "piratedemo/player.h"
#include "piratedemo/types.h"
#include "piratedemo/wall.h"

namespace engine2 {
class Graphics2D;
class SpriteCache;
class TextureCache;
class Window;
}  // namespace engine2

namespace piratedemo {

class Thing;

class Game : public engine2::FrameLoop, public engine2::EventHandler {
 public:
  Game(engine2::Window* window,
       engine2::Graphics2D* graphics,
       engine2::Font* font);
  bool Load();

  // FrameLoop
  void EveryFrame() override;

  // EventHandler
  void OnKeyDown(const SDL_KeyboardEvent& event) override;
  void OnKeyUp(const SDL_KeyboardEvent& event) override;

  engine2::Time last_update_time() const { return last_update_time_; }
  engine2::Graphics2D* graphics() const { return graphics_; }
  engine2::Camera2D<Thing>* camera() { return &camera_; }

 private:
  void MovePlayer(Direction direction, bool key_down);

  engine2::Window* window_;
  engine2::Graphics2D* graphics_;
  engine2::TextureCache texture_cache_;
  engine2::SpriteCache sprite_cache_;
  engine2::Camera2D<Thing> camera_;
  engine2::Timing::FramerateRegulator idler_{60};

  std::unique_ptr<engine2::TileMap> map_;
  Player player_;
  std::list<Wall> walls_;

  engine2::Space<2, Player, Wall> space_;

  engine2::Time last_update_time_{};
  std::vector<Direction> move_keypress_stack_;

#ifdef PERF
  engine2::Time last_perf_flush_{};
  engine2::Time::Delta perf_flush_period_ =
      engine2::Time::Delta::FromSeconds(1);
  std::unique_ptr<engine2::FrameStatsOverlay> frame_stats_overlay_;
#endif
};

}  // namespace piratedemo

#endif  // PIRATEDEMO_GAME_H_