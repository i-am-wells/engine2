#include "piratedemo/data.h"

using namespace engine2;

namespace piratedemo {
namespace {

constexpr Vec<int, 2> kTileSize{16, 16};

constexpr Vec<int, 2> kPlayerSize{16, 32};
constexpr Rect<int, 2> kPlayerHitBox{3, 22, 10, 10};
constexpr Vec<int, 2> kPlayerDrawOffset{-kPlayerHitBox.pos.value[0],
                                        -kPlayerHitBox.pos.value[1]};
constexpr Time::Delta kPlayerFrameDuration =
    Time::Delta::FromMicroseconds(256'000);

Sprite::AnimationFrame MakeFrame(Point<int, 2> point,
                                 Vec<int, 2> size,
                                 Vec<int, 2> draw_offset,
                                 Time::Delta duration) {
  return {/*source_rect=*/{point * size, size}, draw_offset, duration};
}

Sprite::AnimationFrame MakePlayerFrame(Point<int, 2> point) {
  return MakeFrame(point, kPlayerSize, kPlayerDrawOffset, kPlayerFrameDuration);
}

std::string DirectionName(PlayerWalkDirection direction) {
  switch (direction) {
    case PlayerWalkDirection::kSouth:
      return "south";
    case PlayerWalkDirection::kNorth:
      return "north";
    case PlayerWalkDirection::kEast:
      return "east";
    case PlayerWalkDirection::kWest:
      return "west";
  }
}

}  // namespace

SpriteSheetInfo MakeShipSpriteSheet() {
  std::vector<SpriteInfo> sprites;
  for (int y = 0; y < 20; ++y) {
    for (int x = 0; x < 27; ++x) {
      sprites.emplace_back(
          "ship" + std::to_string(x) + "_" + std::to_string(y),
          std::vector<Sprite::AnimationFrame>({MakeFrame(
              {x, y}, kTileSize, /*draw_offset=*/{}, /*duration=*/{})}));
    }
  }

  return {"piratedemo/ship_vertical.png", std::move(sprites)};
}

SpriteSheetInfo MakePlayerSpriteSheet() {
  std::vector<SpriteInfo> sprites;
  for (int x = 0; x < 4; ++x) {
    std::string direction = DirectionName(static_cast<PlayerWalkDirection>(x));

    // Standing
    sprites.emplace_back(
        "stand_" + direction,
        std::vector<Sprite::AnimationFrame>(
            {MakePlayerFrame({x, 1}), MakePlayerFrame({x, 0})}));

    // Walking
    std::vector<Sprite::AnimationFrame> walk_frames;
    for (int y = 1; y < 5; ++y)
      walk_frames.push_back(MakePlayerFrame({x, y}));
    sprites.emplace_back("walk_" + direction, std::move(walk_frames));
  }

  return {"piratedemo/player_sprite.png", std::move(sprites)};
}

}  // namespace piratedemo