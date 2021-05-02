#ifndef PIRATEDEMO_VISIBLE_H_
#define PIRATEDEMO_VISIBLE_H_

#include "engine2/camera2d.h"
#include "engine2/graphics2d.h"
#include "engine2/sprite.h"

namespace piratedemo {

class Game;

class Thing : public engine2::Camera2D<Thing>::Visible,
              public engine2::RectObject<2> {
 public:
  Thing(Game* game,
        engine2::Graphics2D* graphics,
        engine2::Camera2D<Thing>* camera,
        engine2::Sprite* active_sprite,
        const engine2::Rect<>& world_rect,
        double mass_kg);

  void Draw() override;
  bool operator<(const Thing& other) const;

 protected:
  engine2::Sprite* active_sprite_ = nullptr;

 private:
  engine2::Point<> WorldToScreen(const engine2::Point<>& point) const;

  Game* game_;

  // Screen-coords graphics renderer.
  engine2::Graphics2D* graphics_;
  engine2::Camera2D<Thing>* camera_;
};

}  // namespace piratedemo

#endif  // PIRATEDEMO_VISIBLE_H_