#ifndef ENGINE2_TILE_MAP_H_
#define ENGINE2_TILE_MAP_H_

#include <memory>
#include <vector>

#include "engine2/camera2d.h"
#include "engine2/sprite.h"

namespace engine2 {

class TileMap {
 public:
  // TODO: think about layers: may need variable dimensions
  TileMap(const Vec<int, 2>& tile_size,
          const Vec<int64_t, 2>& grid_size,
          const Point<int64_t, 2>& position_in_world,
          bool empty_initialize = false);

  // note: camera should know where it is in the world (?)
  void Draw(Camera2D* camera);

  // Add a sprite to the list of sprites available for this map. Returns the
  // sprite's index.
  int AddSprite(Sprite* sprite);

  // Set a grid cell to be drawn with the sprite corresponding to
  // |sprite_index|.
  void SetTile(const Point<int64_t, 2>& tile_location, int sprite_index);

 private:
  struct Tile {
    uint16_t sprite_index;
    // TODO does any per-tile state belong here?
  };
  Tile& GetTile(const Point<int64_t, 2>& grid_position);
  bool PositionInMap(const Point<int64_t, 2>& grid_position) const;

  Vec<int64_t, 2> tile_size_;
  Vec<int64_t, 2> grid_size_;
  Rect<int64_t, 2> world_rect_;

  std::vector<Sprite*> sprites_;
  std::unique_ptr<Tile[]> tiles_;
};

}  // namespace engine2

#endif  // ENGINE2_TILE_MAP_H_