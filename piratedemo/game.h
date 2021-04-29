#ifndef PIRATEDEMO_GAME_H_
#define PIRATEDEMO_GAME_H_

#include <memory>

#include "engine2/camera2d.h"
#include "engine2/event_handler.h"
#include "engine2/frame_loop.h"
#include "engine2/space.h"
#include "engine2/tile_map.h"

#include "piratedemo/player.h"

namespace engine2 {
class Graphics2D;
class SpriteCache;
class TextureCache;
class Window;
}  // namespace engine2

namespace piratedemo {

class Game : public engine2::FrameLoop, public engine2::EventHandler {
 public:
  Game(engine2::Window* window, engine2::Graphics2D* graphics);
  bool Load();

  // FrameLoop
  void EveryFrame() override;

  // EventHandler
  // TODO

 private:
  engine2::Window* window_;
  engine2::Graphics2D* graphics_;
  engine2::TextureCache texture_cache_;
  engine2::SpriteCache sprite_cache_;
  engine2::Camera2D camera_;

  std::unique_ptr<engine2::TileMap> map_;
  Player player_;
};

}  // namespace piratedemo

#endif  // PIRATEDEMO_GAME_H_