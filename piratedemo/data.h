#ifndef PIRATEDEMO_DATA_H_
#define PIRATEDEMO_DATA_H_

#include "engine2/data.h"

namespace piratedemo {

enum class PlayerWalkDirection { kSouth, kNorth, kEast, kWest };

engine2::SpriteSheetInfo MakeShipSpriteSheet();
engine2::SpriteSheetInfo MakePlayerSpriteSheet();

}  // namespace piratedemo

#endif  // PIRATEDEMO_DATA_H_