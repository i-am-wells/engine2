#include <cstring>
#include <sstream>

extern "C" {
#include <endian.h>
}

#include "engine2/camera2d.h"
#include "engine2/sprite_cache.h"
#include "engine2/tile_map.h"

namespace engine2 {
namespace {

// The endianness conversion functions are wrapped like this because I can't
// seem to pass them as function pointers directly.
uint16_t Le16ToH(uint16_t val) {
  return le16toh(val);
}
uint32_t Le32ToH(uint32_t val) {
  return le32toh(val);
}
uint64_t Le64ToH(uint64_t val) {
  return le64toh(val);
}
uint16_t HToLe16(uint16_t val) {
  return htole16(val);
}
uint32_t HToLe32(uint32_t val) {
  return htole32(val);
}
uint64_t HToLe64(uint64_t val) {
  return htole64(val);
}

template <class T>
bool ReadInt(std::istream& stream, T& out, T (*conv)(T)) {
  out = 0;
  for (int i = 0; i < sizeof(T); ++i)
    out |= (T(stream.get()) << (i * 8));
  out = conv(out);
  return stream.good();
}

bool ReadInt16(std::istream& stream, uint16_t& out) {
  return ReadInt(stream, out, Le16ToH);
}
bool ReadInt32(std::istream& stream, uint32_t& out) {
  return ReadInt(stream, out, Le32ToH);
}
bool ReadInt64(std::istream& stream, uint64_t& out) {
  return ReadInt(stream, out, Le64ToH);
}

bool ReadVec2Int64(std::istream& stream, Vec<int64_t, 2>& out) {
  uint64_t x, y;
  if (!ReadInt64(stream, x) || !ReadInt64(stream, y))
    return false;
  out[0] = x;
  out[1] = y;
  return true;
}

bool ReadString(std::istream& stream, std::string& str) {
  uint32_t length;
  if (!ReadInt32(stream, length))
    return false;

  auto data = std::make_unique<char[]>(length + 1);
  data.get()[length] = '\0';

  stream.read(data.get(), length);
  str.assign(data.get(), length);

  return stream.good();
}

template <class T>
bool WriteInt(std::ostream& stream, T val, T (*conv)(T)) {
  T le_val = conv(val);
  for (int i = 0; i < sizeof(T); ++i)
    stream.put((le_val >> (i * 8)) & 0xff);
  return stream.good();
}

bool WriteInt16(std::ostream& stream, uint16_t val) {
  return WriteInt(stream, val, HToLe16);
}
bool WriteInt32(std::ostream& stream, uint32_t val) {
  return WriteInt(stream, val, HToLe32);
}
bool WriteInt64(std::ostream& stream, uint64_t val) {
  return WriteInt(stream, val, HToLe64);
}

bool WriteVec2Int64(std::ostream& stream, const Vec<int64_t, 2>& val) {
  return WriteInt64(stream, val[0]) && WriteInt64(stream, val[1]);
}

bool WriteString(std::ostream& stream, const std::string& str) {
  if (!WriteInt32(stream, str.length()))
    return false;
  stream.write(str.data(), str.length());
  return stream.good();
}

}  // namespace

// static
std::unique_ptr<TileMap> TileMap::FromString(const std::string& data,
                                             SpriteCache* sprite_cache) {
  std::istringstream stream(data);
  return Read(stream, sprite_cache);
}

std::unique_ptr<TileMap> TileMap::Read(std::istream& stream,
                                       SpriteCache* sprite_cache) {
  Vec<int64_t, 2> tile_size, grid_size, position_in_world;
  uint32_t layer_count, tile_vector_size;
  if (!ReadVec2Int64(stream, tile_size) || !ReadVec2Int64(stream, grid_size) ||
      !ReadVec2Int64(stream, position_in_world) ||
      !ReadInt32(stream, layer_count) || !ReadInt32(stream, tile_vector_size)) {
    return nullptr;
  }

  auto map = std::make_unique<TileMap>(tile_size, grid_size, layer_count,
                                       position_in_world, sprite_cache);

  for (int i = 0; i < tile_vector_size; ++i) {
    std::string sprite_path;
    if (!ReadString(stream, sprite_path))
      return nullptr;

    uint32_t animation_offset_ms;
    if (!ReadInt32(stream, animation_offset_ms))
      return nullptr;

    Sprite* sprite = nullptr;
    if (!sprite_path.empty()) {
      sprite = sprite_cache->Get(sprite_path);
      if (!sprite)
        return nullptr;
    }

    map->AddTile(
        {sprite, Time::Delta::FromMicroseconds(animation_offset_ms * 1000)});
  }

  TileMap::GridPoint p;
  for (p.y() = 0; p.y() < grid_size.y(); ++p.y()) {
    for (p.x() = 0; p.x() < grid_size.x(); ++p.x()) {
      for (int layer = 0; layer < layer_count; ++layer) {
        uint16_t index;
        if (!ReadInt16(stream, index))
          return nullptr;

        map->SetTileIndex(p, layer, index);
      }
    }
  }

  return std::move(map);
}

bool TileMap::Write(std::ostream& stream) const {
  if (!WriteVec2Int64(stream, tile_size_) ||
      !WriteVec2Int64(stream, grid_size_) ||
      !WriteVec2Int64(stream, world_rect_.pos) ||
      !WriteInt32(stream, layer_count_) || !WriteInt32(stream, tiles_.size())) {
    return false;
  }

  for (const Tile& tile : tiles_) {
    // If sprite is null, record an empty string.
    auto sprite_name = std::make_optional<std::string>();
    if (tile.sprite) {
      sprite_name = sprite_cache_->LookupName(tile.sprite);
      if (!sprite_name)
        return false;
    }

    if (!WriteString(stream, sprite_name.value()) ||
        !WriteInt32(stream, tile.animation_offset.ToMicroseconds() / 1000)) {
      return false;
    }
  }

  TileMap::GridPoint p;
  for (p.y() = 0; p.y() < grid_size_.y(); ++p.y()) {
    for (p.x() = 0; p.x() < grid_size_.x(); ++p.x()) {
      for (int layer = 0; layer < layer_count_; ++layer) {
        if (!WriteInt16(stream, GetTileIndex(p, layer)))
          return false;
      }
    }
  }
  return true;
}

TileMap::TileMap(const Vec<int64_t, 2>& tile_size,
                 const Vec<int64_t, 2>& grid_size,
                 int layer_count,
                 const Point<int64_t, 2>& position_in_world,
                 SpriteCache* sprite_cache,
                 bool empty_initialize)
    : sprite_cache_(sprite_cache),
      tile_size_(tile_size.template ConvertTo<int64_t>()),
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

  // Skip drawing beyond the edge of the map
  if (corner0.x() < 0)
    corner0.x() = 0;

  if (corner0.x() >= grid_size_.x())
    return;

  if (corner0.y() < 0)
    corner0.y() = 0;

  if (corner0.y() >= grid_size_.y())
    return;

  if (corner1.x() < 0)
    corner1.x() = 0;

  if (corner1.x() >= grid_size_.x())
    corner1.x() = grid_size_.x() - 1;

  if (corner1.y() < 0)
    corner1.y() = 0;

  if (corner1.y() >= grid_size_.y())
    corner1.y() = grid_size_.y() - 1;

  Vec<double, 2> scale{scale_, scale_};

  // grid point to screen:
  // ((point - corner0) * tile_size - offset) * scale_
  Vec<int64_t, 2> offset = (world_rect.pos - world_rect_.pos) % tile_size_;

  for (int i = first_layer; i < after_last_layer; ++i) {
    GridPoint point;
    for (point.y() = corner0.y(); point.y() <= corner1.y(); ++point.y()) {
      for (point.x() = corner0.x(); point.x() <= corner1.x(); ++point.x()) {
        Point<> draw_point = ((point - corner0) * tile_size_ + world_rect_.pos -
                              world_rect.pos) *
                             scale;

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

void TileMap::SetTile(uint16_t index, const Tile& tile) {
  if (index >= tiles_.size())
    tiles_.resize(index + 1);
  tiles_[index] = tile;
}

uint16_t TileMap::GetTileCount() const {
  return tiles_.size();
}

uint64_t TileMap::GridIndex(const GridPoint& grid_point, int layer) const {
  return layer_count_ * (grid_size_.x() * grid_point.y() + grid_point.x()) +
         layer;
}

}  // namespace engine2
