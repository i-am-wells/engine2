#ifndef PIRATEDEMO_PLAYER_H_
#define PIRATEDEMO_PLAYER_H_

#include "engine2/rect_object.h"
#include "engine2/sprite.h"

namespace engine2 {
class Sprite;
class SpriteCache;
}  // namespace engine2

namespace piratedemo {

class Player : public engine2::RectObject<2> {
 public:
  Player(engine2::SpriteCache* sprite_cache);
  bool Load();

  void Update(const engine2::Time::Delta& delta) override;

 private:
  std::array<engine2::Sprite, 8> sprites_;
  engine2::SpriteCache* sprite_cache_;
};

}  // namespace piratedemo

#endif  // PIRATEDEMO_PLAYER_H_