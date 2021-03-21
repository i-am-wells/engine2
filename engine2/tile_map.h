#ifndef ENGINE2_TILE_MAP_H_
#define ENGINE2_TILE_MAP_H_

#include <list>
#include <memory>
#include <vector>

#include "engine2/camera2d.h"
#include "engine2/sprite.h"
#include "engine2/sprite_cache.h"
#include "engine2/time.h"

namespace engine2 {

class TileMap {
 public:
  static constexpr int kAllLayers = -1;

  static std::unique_ptr<TileMap> FromString(const std::string& data,
                                             SpriteCache* sprite_cache);

  static std::unique_ptr<TileMap> Read(std::istream& stream,
                                       SpriteCache* sprite_cache);
  bool Write(std::ostream& stream) const;

  TileMap(const Vec<int64_t, 2>& tile_size,
          const Vec<int64_t, 2>& grid_size,
          int layer_count,
          const Point<int64_t, 2>& position_in_world,
          SpriteCache* sprite_cache,
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
  Point<> GridToWorld(const GridPoint& grid_point) const;

  // Returns nullptr if point/layer are out of bounds.
  Tile* GetTile(const GridPoint& point, int layer);
  Tile* GetTileByIndex(uint16_t tile_index);

  uint16_t GetTileIndex(const GridPoint& point, int layer) const;
  void SetTileIndex(const GridPoint& point, int layer, uint16_t tile_index);

  // Add a tile to the map's set of tiles and return the index.
  uint16_t AddTile(const Tile& tile);
  void AddTiles(const std::vector<Tile>& tiles);
  uint16_t GetTileCount() const;
  // If index is out of bounds, resizes tile vector and fills in empty tiles.
  void SetTile(uint16_t index, const Tile& tile);

  double GetScale() const { return scale_; }
  void SetScale(double scale) { scale_ = scale; }

  Vec<int64_t, 2> GetTileSize() const { return tile_size_; }
  Vec<int64_t, 2> GetGridSize() const { return grid_size_; }
  int GetLayerCount() const { return layer_count_; }
  Rect<int64_t, 2> GetWorldRect() const { return world_rect_; }
  void SetWorldRect(const engine2::Rect<>& rect) { world_rect_ = rect; }

 private:
  bool PositionInMap(const GridPoint& grid_position) const;

  uint64_t GridIndex(const GridPoint& grid_point, int layer) const;

  SpriteCache* sprite_cache_;
  Vec<int64_t, 2> tile_size_;
  Vec<int64_t, 2> grid_size_;
  Rect<int64_t, 2> world_rect_;
  int layer_count_;

  // used for drawing
  double scale_ = 1.;

  // Storage for tiles.
  std::vector<Tile> tiles_;

  // Stores indices of tiles.
  std::unique_ptr<uint16_t[]> grid_;
};

}  // namespace engine2

#endif  // ENGINE2_TILE_MAP_H_