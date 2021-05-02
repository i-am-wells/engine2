#include "piratedemo/wall.h"
#include "piratedemo/game.h"

using namespace engine2;

namespace piratedemo {

Wall::Wall(Game* game, const Rect<>& world_rect)
    : Thing(game, game->graphics(), game->camera(), nullptr, world_rect, 1) {}

}  // namespace piratedemo