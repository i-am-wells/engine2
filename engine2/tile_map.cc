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
  int first_layer, after_last_layer;
  if (layer == kAllLayers) {
    first_layer = 0;
    after_last_layer = layer_count_;
  } else {
    first_layer = layer;
    after_last_layer = layer + 1;
  }

  GridPoint corner0 = WorldToGrid(world_rect.pos);
  GridPoint corner1 = WorldToGrid(world_rect.pos + world_rect.size);

  // grid point to screen:
  // ((point - corner0) * tile_size - offset) * scale_
  Vec<int64_t, 2> offset = world_rect.pos % tile_size_;

  GridPoint point;
  for (point.y() = corner0.y(); point.y() <= corner1.y(); ++point.y()) {
    for (point.x() = corner0.x(); point.x() <= corner1.x(); ++point.x()) {
      // TODO
      //
      Point<> draw_point =
          (((point - corner0) * tile_size_ - offset).ConvertTo<double>() *
           scale_)
              .ConvertTo<int64_t>();

      for (int i = first_layer; i < after_last_layer; ++i) {
        Tile* tile = GetTile(point, i);
        if (tile && tile->sprite) {
          if (scale_ == 1.)
            tile->sprite->Draw(graphics, draw_point);
          else
            tile->sprite->Draw(graphics, draw_point, scale_);
        }
      }
      //
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

Point<> TileMap::GridToWorld(const GridPoint& grid_point) const {
  return (grid_point * tile_size_) + world_rect_.pos;
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

uint16_t TileMap::GetTileCount() const {
  return tiles_.size();
}

uint64_t TileMap::GridIndex(const GridPoint& grid_point, int layer) const {
  return layer_count_ * (grid_size_.x() * grid_point.y() + grid_point.x()) +
         layer;
}

}  // namespace engine2
