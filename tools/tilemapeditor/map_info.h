#include <memory>
#include <string>
#include <vector>

#include "engine2/sprite_cache.h"
#include "engine2/tile_map.h"
#include "engine2/vec.h"

namespace tilemapeditor {

using engine2::Vec;

struct TileInfo {
  std::string sprite_name;
  int animation_offset_ms;
  std::vector<std::string> tags;
};

struct MapInfo {
  int layers;
  Vec<int64_t, 2> grid_size;
  Vec<int64_t, 2> world_position;
  Vec<int, 2> tile_size;
  std::vector<TileInfo> tiles;
  std::vector<std::string> tags;

  std::unique_ptr<engine2::TileMap> CreateTileMap(
      engine2::SpriteCache* sprite_cache) const;
};

std::unique_ptr<MapInfo> ReadMapInfoFromFile(const std::string& file_path);

}  // namespace tilemapeditor