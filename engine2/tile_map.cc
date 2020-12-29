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
  tiles_ = std::make_unique<Tile[]>(num_tiles);

  if (empty_initialize)
    std::memset(tiles_.get(), 0, num_tiles);
}

void TileMap::Draw(Camera2D* camera) {
  Rect<int64_t, 2> camera_world_rect = camera->GetRect();
  Point<int64_t, 2> draw_point;

  for (int64_t y = 0; y <= camera_world_rect.h(); y += tile_size_.y()) {
    draw_point.y() = y + camera_world_rect.y();

    for (int64_t x = 0; x <= camera_world_rect.w(); x += tile_size_.x()) {
      draw_point.x() = x + camera_world_rect.x();

      Point<int64_t, 2> grid_point =
          (draw_point - world_rect_.pos) / tile_size_;

      Tile& tile = GetTile(grid_point);
      sprites_[tile.sprite_index]->Draw(camera->InWorldCoords(), draw_point);
    }
  }
}

int TileMap::AddSprite(Sprite* sprite) {
  sprites_.push_back(sprite);
  return sprites_.size() - 1;
}

void TileMap::SetTile(const Point<int64_t, 2>& tile_location,
                      int sprite_index) {
  if (!PositionInMap(tile_location))
    return;
  GetTile(tile_location).sprite_index = sprite_index;
}

TileMap::Tile& TileMap::GetTile(const Point<int64_t, 2>& grid_position) {
  return tiles_[grid_position.y() * grid_size_.x() + grid_position.x()];
}

bool TileMap::PositionInMap(const Point<int64_t, 2>& grid_position) const {
  // TODO N dimensions?
  for (int i = 0; i < 2; ++i) {
    if (grid_position[i] < 0 || grid_position[i] >= grid_size_[i])
      return false;
  }
  return true;
}

}  // namespace engine2