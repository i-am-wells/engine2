#include "piratedemo/player.h"
#include "engine2/sprite_cache.h"

using namespace engine2;

namespace piratedemo {

Player::Player(SpriteCache* sprite_cache)
    : RectObject<2>({0, 0, 32, 64}, /*mass=*/75), sprite_cache_(sprite_cache) {}

bool Player::Load() {
  sprite_ = sprite_cache_->Get(
}

}  // namespace piratedemo