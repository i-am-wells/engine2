#include <cstring>

#include "engine2/camera2d.h"
#include "engine2/tile_map.h"

namespace engine2 {

TileMap::TileMap(const Vec<int, 2>& tile_size,
                 const Vec<int64_t, 2>& grid_size,
                 int layer_count,
                 const Point<int64_t, 2>& position_in_world,
                 bool empty_initialize)
    : tile_size_(tile_size.template ConvertTo<int64_t>()),
      grid_size_(grid_size),
      layer_count_(layer_count) {
  world_rect_ = Rect<int64_t, 2>{position_in_world, grid_size * tile_size_};
  size_t num_tiles = grid_size.x() * grid_size.y() * layer_count_;
  grid_ = std::make_unique<uint16_t[]>(num_tiles);

  if (empty_initialize)
    std::memset(grid_.get(), 0, num_tiles * sizeof(uint16_t));
}

void TileMap::Draw(Camera2D* camera, int layer) {
  Draw(camera->InWorldCoords(), camera->GetRect());
}

void TileMap::Draw(Graphics2D* graphics,
                   const Rect<int64_t, 2>& world_rect,
                   int layer) {
  Point<> draw_offset = world_rect.pos % tile_size_;
  Point<> draw_point;

  int first_layer, after_last_layer;
  if (layer == kAllLayers) {
    first_layer = 0;
    after_last_layer = layer_count_;
  } else {
    first_layer = layer;
    after_last_layer = layer + 1;
  }

  for (draw_point.y() = -draw_offset.y(); draw_point.y() <= world_rect.h();
       draw_point.y() += tile_size_.y()) {
    for (draw_point.x() = -draw_offset.x(); draw_point.x() <= world_rect.w();
         draw_point.x() += tile_size_.x()) {
      Point<> world_point = draw_point + world_rect.pos;
      GridPoint grid_point = WorldToGrid(world_point);

      for (int i = first_layer; i < after_last_layer; ++i) {
        Tile* tile = GetTile(grid_point, i);
        if (tile && tile->sprite)
          tile->sprite->Draw(graphics, draw_point);
      }
    }
  }
}

bool TileMap::PositionInMap(const GridPoint& grid_position) const {
  for (int i = 0; i < 2; ++i) {
    if (grid_position[i] < 0 || grid_position[i] >= grid_size_[i])
      return false;
  }
  return true;
}

TileMap::GridPoint TileMap::WorldToGrid(
    const Point<int64_t, 2>& world_point) const {
  Point<> temp = (world_point - world_rect_.pos) / tile_size_;
  GridPoint result;
  result.value[0] = temp.value[0];
  result.value[1] = temp.value[1];
  return result;
}

TileMap::Tile* TileMap::GetTile(const GridPoint& grid_point, int layer) {
  if (!PositionInMap(grid_point) || layer < 0 || layer >= layer_count_)
    return nullptr;

  uint16_t index = GetTileIndex(grid_point, layer);
  if (index >= tiles_.size())
    return nullptr;

  return &(tiles_[index]);
}

TileMap::Tile* TileMap::GetTileByIndex(uint16_t tile_index) {
  if (tile_index > tiles_.size())
    return nullptr;

  return &(tiles_[tile_index]);
}

uint16_t TileMap::GetTileIndex(const GridPoint& grid_point, int layer) const {
  return grid_[GridIndex(grid_point, layer)];
}

void TileMap::SetTileIndex(const GridPoint& grid_point,
                           int layer,
                           uint16_t tile_index) {
  if (!PositionInMap(grid_point) || layer < 0 || layer >= layer_count_ ||
      tile_index >= tiles_.size()) {
    return;
  }

  grid_[GridIndex(grid_point, layer)] = tile_index;
}

uint16_t TileMap::AddTile(const Tile& tile) {
  tiles_.push_back(tile);
  return tiles_.size() - 1;
}

void TileMap::AddTiles(const std::vector<Tile>& tiles) {
  tiles_.insert(tiles_.end(), tiles.begin(), tiles.end());
}

uint64_t TileMap::GridIndex(const GridPoint& grid_point, int layer) const {
  return layer_count_ * (grid_size_.x() * grid_point.y() + grid_point.x()) +
         layer;
}

}  // namespace engine2