#ifndef ENGINE2_TILE_MAP_H_
#define ENGINE2_TILE_MAP_H_

#include <list>
#include <memory>
#include <vector>

#include "engine2/camera2d.h"
#include "engine2/sprite.h"
#include "engine2/time.h"

namespace engine2 {

class TileMap {
 public:
  static constexpr int kAllLayers = -1;

  TileMap(const Vec<int, 2>& tile_size,
          const Vec<int64_t, 2>& grid_size,
          int layer_count,
          const Point<int64_t, 2>& position_in_world,
          bool empty_initialize = false);

  void Draw(Graphics2D* graphics,
            const Rect<int64_t, 2>& world_rect,
            int layer = kAllLayers);
  void Draw(Camera2D* camera, int layer = kAllLayers);

  struct Tile {
    Sprite* sprite;
    Time::Delta animation_offset{};
  };

  // We create a special type for points on the grid to make it harder to
  // accidentally pass a point in the world to a function that expects tile grid
  // coordinates.
  struct GridPoint : public Point<> {};

  // Convert a point in the world to tile grid coordinates.
  GridPoint WorldToGrid(const Point<>& world_point) const;

  // Returns nullptr if point/layer are out of bounds.
  Tile* GetTile(const GridPoint& point, int layer);
  Tile* GetTileByIndex(uint16_t tile_index);

  uint16_t GetTileIndex(const GridPoint& point, int layer) const;
  void SetTileIndex(const GridPoint& point, int layer, uint16_t tile_index);

  // Add a tile to the map's set of tiles and return the index.
  uint16_t AddTile(const Tile& tile);
  void AddTiles(const std::vector<Tile>& tiles);

 private:
  bool PositionInMap(const GridPoint& grid_position) const;

  uint64_t GridIndex(const GridPoint& grid_point, int layer) const;

  Vec<int64_t, 2> tile_size_;
  Vec<int64_t, 2> grid_size_;
  Rect<int64_t, 2> world_rect_;
  int layer_count_;

  // Storage for tiles.
  std::vector<Tile> tiles_;

  // Stores indices of tiles.
  std::unique_ptr<uint16_t[]> grid_;
};

}  // namespace engine2

#endif  // ENGINE2_TILE_MAP_H_