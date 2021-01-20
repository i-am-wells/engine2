#include <cstring>

#include "engine2/camera2d.h"
#include "engine2/tile_map.h"

namespace engine2 {

TileMap::TileMap(const Vec<int, 2>& tile_size,
                 const Vec<int64_t, 2>& grid_size,
                 const Point<int64_t, 2>& position_in_world,
                 bool empty_initialize)
    : tile_size_(tile_size.template ConvertTo<int64_t>()),
      grid_size_(grid_size) {
  world_rect_ = Rect<int64_t, 2>{position_in_world, grid_size * tile_size_};
  size_t num_tiles = grid_size.x() * grid_size.y();
  grid_ = std::make_unique<uint16_t[]>(num_tiles);

  if (empty_initialize)
    std::memset(grid_.get(), 0, num_tiles * sizeof(uint16_t));
}

void TileMap::Draw(Camera2D* camera) {
  Draw(camera->InWorldCoords(), camera->GetRect());
}

void TileMap::Draw(Graphics2D* graphics, const Rect<int64_t, 2>& world_rect) {
  Point<int64_t, 2> draw_point{};

  for (int64_t y = 0; y <= world_rect.h(); y += tile_size_.y()) {
    draw_point.y() = y + world_rect.y();

    for (int64_t x = 0; x <= world_rect.w(); x += tile_size_.x()) {
      draw_point.x() = x + world_rect.x();

      if (!world_rect_.Contains(draw_point))
        continue;

      GetTileStackAtWorldPosition(draw_point).Draw(graphics, draw_point);
    }
  }
}

void TileMap::TileStack::Draw(Graphics2D* graphics,
                              const Point<int64_t, 2>& dest) {
  for (Tile& tile : tiles)
    tile.sprite->Draw(graphics, dest);
}

std::pair<TileMap::TileStackReference, int> TileMap::AddTileStack() {
  tile_stacks_.push_front({});
  tile_stack_refs_.push_back(tile_stacks_.begin());
  return {tile_stacks_.begin(), tile_stack_refs_.size() - 1};
}

void TileMap::AddTileStacks(const std::vector<TileStack>& stacks) {
  for (const TileStack& stack : stacks)
    *(AddTileStack().first) = stack;
}

void TileMap::SetTileStackAtGridPosition(const Point<int64_t, 2>& grid_position,
                                         int tile_stack_index) {
  grid_[grid_position.y() * grid_size_.x() + grid_position.x()] =
      tile_stack_index;
}

void TileMap::SetTileStackAtWorldPosition(
    const Point<int64_t, 2>& world_position,
    int tile_stack_index) {
  SetTileStackAtGridPosition(WorldToGrid(world_position), tile_stack_index);
}

TileMap::TileStack& TileMap::GetTileStackAtGridPosition(
    const Point<int64_t, 2>& grid_position) {
  return *(tile_stack_refs_[grid_[grid_position.y() * grid_size_.x() +
                                  grid_position.x()]]);
}

TileMap::TileStack& TileMap::GetTileStackAtWorldPosition(
    const Point<int64_t, 2>& world_position) {
  return GetTileStackAtGridPosition(WorldToGrid(world_position));
}

bool TileMap::PositionInMap(const Point<int64_t, 2>& grid_position) const {
  for (int i = 0; i < 2; ++i) {
    if (grid_position[i] < 0 || grid_position[i] >= grid_size_[i])
      return false;
  }
  return true;
}

Point<int64_t, 2> TileMap::WorldToGrid(
    const Point<int64_t, 2>& world_pos) const {
  return (world_pos - world_rect_.pos) / tile_size_;
}

}  // namespace engine2