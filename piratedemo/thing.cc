#include "piratedemo/thing.h"
#include "engine2/camera2d.h"
#include "piratedemo/game.h"

using namespace engine2;

namespace piratedemo {

Thing::Thing(Game* game,
             Graphics2D* graphics,
             Camera2D<Thing>* camera,
             Sprite* active_sprite,
             const Rect<>& world_rect,
             double mass_kg)
    : RectObject<2>(world_rect, mass_kg),
      game_(game),
      graphics_(graphics),
      camera_(camera),
      active_sprite_(active_sprite) {}

void Thing::Draw() {
  if (!active_sprite_)
    return;

  active_sprite_->Draw(graphics_, WorldToScreen(rect_.pos));
  active_sprite_->Update(game_->last_update_time());
}

bool Thing::operator<(const Thing& other) const {
  return rect_.y() < other.rect_.y();
}

Point<> Thing::WorldToScreen(const Point<>& point) const {
  return point - camera_->GetRect().pos;
}

}  // namespace piratedemo