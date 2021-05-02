#ifndef ENGINE2_DATA_H_
#define ENGINE2_DATA_H_

#include <string>
#include <vector>

#include "engine2/rect.h"
#include "engine2/sprite.h"
#include "engine2/time.h"
#include "engine2/vec.h"

namespace engine2 {

struct TileInfo {
  std::string_view sprite_name;
  Time::Delta animation_offset;
  std::vector<std::string> tags;
};

struct MapInfo {
  int layers;
  Vec<int64_t, 2> grid_size;
  Vec<int64_t, 2> world_position;
  Vec<int, 2> tile_size;
  std::vector<TileInfo> tiles;
  std::vector<std::string> tags;
};

struct SpriteInfo {
  std::string name;
  std::vector<Sprite::AnimationFrame> frames;
};

struct SpriteSheetInfo {
  std::string image_path;
  std::vector<SpriteInfo> sprites;
};

}  // namespace engine2

#endif  // ENGINE2_DATA_H_