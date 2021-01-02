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
  TileMap(const Vec<int, 2>& tile_size,
          const Vec<int64_t, 2>& grid_size,
          const Point<int64_t, 2>& position_in_world,
          bool empty_initialize = false);

  void Draw(Camera2D* camera);

  struct Tile {
    Sprite* sprite;
    Time::Delta animation_offset;
  };

  struct TileStack {
    std::vector<Tile> tiles;
    void Draw(Graphics2D* graphics, const Point<int64_t, 2>& dest);
  };

  using TileStackReference = std::list<TileStack>::iterator;

  // Create a new TileStack that can be referenced by grid cells.
  std::pair<TileStackReference, int> AddTileStack();
  // Remove a TileStack.
  void RemoveTileStack(TileStackReference ref);

  // Returns a the TileStack at the specified world position.
  TileStack& GetTileStackAtWorldPosition(
      const Point<int64_t, 2>& tile_location);

  // Set a grid cell to be drawn with the TileStack corresponding to
  // |tile_stack_index|.
  void SetTileStackAtGridPosition(const Point<int64_t, 2>& grid_position,
                                  int tile_stack_index);
  void SetTileStackAtWorldPosition(const Point<int64_t, 2>& world_position,
                                   int tile_stack_index);

 private:
  TileStack& GetTileStackAtGridPosition(const Point<int64_t, 2>& grid_position);
  bool PositionInMap(const Point<int64_t, 2>& grid_position) const;

  Point<int64_t, 2> WorldToGrid(const Point<int64_t, 2>& world_pos) const;

  Vec<int64_t, 2> tile_size_;
  Vec<int64_t, 2> grid_size_;
  Rect<int64_t, 2> world_rect_;

  std::list<TileStack> tile_stacks_;

  // Points to tile_stacks_.
  std::vector<TileStackReference> tile_stack_refs_;

  // Points to tile_refs_.
  std::unique_ptr<uint16_t[]> grid_;
};

}  // namespace engine2

#endif  // ENGINE2_TILE_MAP_H_