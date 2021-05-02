#ifndef PIRATEDEMO_PLAYER_H_
#define PIRATEDEMO_PLAYER_H_

#include <array>

#include "engine2/camera2d.h"
#include "engine2/graphics2d.h"
#include "engine2/rect_object.h"
#include "engine2/sprite.h"
#include "piratedemo/thing.h"
#include "piratedemo/types.h"

namespace engine2 {
class Sprite;
class SpriteCache;
}  // namespace engine2

namespace piratedemo {

class Game;
class Wall;

class Player : public Thing {
 public:
  enum class Movement { kStand, kWalk };

  // Load player sprites.
  static bool Load(engine2::Graphics2D* graphics);

  Player(Game* game,
         const engine2::Point<>& start_point,
         engine2::Graphics2D* graphics,
         engine2::Camera2D<Thing>* camera);

  void Face(Direction direction);
  void SetMovement(Movement movement);

  // for Space
  void OnCollideWith(const Player& other,
                     const engine2::Vec<double, 2>& initial_velocity,
                     int dimension) {}
  void OnCollideWith(const Wall& wall,
                     const engine2::Vec<double, 2>& initial_velocity,
                     int dimension);

  engine2::Vec<double, 2>& velocity();

 private:
  void SetSprite();

  static std::unique_ptr<engine2::Texture> sTexture;
  static std::array<engine2::Sprite, 8> sSprites;

  int hp_ = 100;
  Direction direction_;
  Movement movement_;
};

}  // namespace piratedemo

#endif  // PIRATEDEMO_PLAYER_H_