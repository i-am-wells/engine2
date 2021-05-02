#ifndef PIRATEDEMO_WALL_H_
#define PIRATEDEMO_WALL_H_

#include "piratedemo/thing.h"

namespace piratedemo {

class Game;
class Player;

class Wall : public Thing {
 public:
  Wall(Game* game, const engine2::Rect<>& world_rect);

  // for Space
  void OnCollideWith(const Player& player,
                     const engine2::Vec<double, 2>& initial_velocity,
                     int dimension) {}
  void OnCollideWith(const Wall& other,
                     const engine2::Vec<double, 2>& initial_velocity,
                     int dimension) {}
};

}  // namespace piratedemo

#endif  // PIRATEDEMO_WALL_H_