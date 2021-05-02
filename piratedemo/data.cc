#include <string>

#include "piratedemo/data.h"

using namespace engine2;

namespace piratedemo {
namespace {

constexpr Vec<int, 2> kTileSize{16, 16};

Sprite::AnimationFrame MakeFrame(Point<int, 2> point,
                                 Vec<int, 2> size,
                                 Vec<int, 2> draw_offset,
                                 Time::Delta duration) {
  return {/*source_rect=*/{point * size, size}, draw_offset, duration};
}

}  // namespace

SpriteSheetInfo MakeShipSpriteSheet() {
  std::vector<SpriteInfo> sprites;
  for (int y = 0; y < 20; ++y) {
    for (int x = 0; x < 27; ++x) {
      sprites.push_back(
          {"ship" + std::to_string(x) + "_" + std::to_string(y),
           std::vector<Sprite::AnimationFrame>({MakeFrame(
               {x, y}, kTileSize, /*draw_offset=*/{}, /*duration=*/{})})});
    }
  }

  return {"piratedemo/ship_vertical.png", std::move(sprites)};
}

}  // namespace piratedemo