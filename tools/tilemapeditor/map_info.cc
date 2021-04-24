#include "tools/tilemapeditor/map_info.h"
#include "engine2/luadata_util.h"
#include "engine2/sprite.h"
#include "engine2/tile_map.h"
#include "engine2/time.h"
#include "luadata/object.h"
#include "luadata/root_object.h"

namespace tilemapeditor {

using engine2::LuaDataUtil;
using engine2::Sprite;
using engine2::SpriteCache;
using engine2::TileMap;
using engine2::Time;

using luadata::Object;
using luadata::RootObject;

namespace {

TileInfo GetTile(const Object& object, int index) {
  auto obj = object.GetObject(index);
  if (!obj)
    return {};

  std::vector<std::string> tags;
  auto tags_obj = obj->GetObject("tags");
  if (tags_obj) {
    int tags_count = tags_obj->Count();
    tags.reserve(tags_count);
    for (int i = 1; i <= tags_count; ++i)
      tags.push_back(tags_obj->GetString(i));
  }

  return {obj->GetString("sprite_name"), obj->GetInt("animation_offset_ms"),
          std::move(tags)};
}

}  // namespace

std::unique_ptr<TileMap> MapInfo::CreateTileMap(
    SpriteCache* sprite_cache) const {
  auto map = std::make_unique<TileMap>(tile_size, grid_size, layers,
                                       world_position, sprite_cache);
  map->AddTile({nullptr});

  std::map<std::string, int> tag_to_id;
  int id = 0;
  for (auto& tag : tags)
    tag_to_id[tag] = id++;

  for (const TileInfo& tile : tiles) {
    std::bitset<64> tile_tags;
    for (const std::string& tile_tag : tile.tags) {
      auto iter = tag_to_id.find(tile_tag);
      if (iter != tag_to_id.end())
        tile_tags.set(iter->second);
    }

    Sprite* sprite = sprite_cache->Get(tile.sprite_name);
    map->AddTile(
        {sprite, Time::Delta::FromMicroseconds(tile.animation_offset_ms * 1000),
         tile_tags});
  }
  return map;
}

std::unique_ptr<MapInfo> ReadMapInfoFromFile(const std::string& file_path) {
  auto [root, error] = RootObject::LoadFile(file_path);
  if (!root)
    return nullptr;

  auto info = std::make_unique<MapInfo>();
  info->layers = root->GetInt("layers", 1);
  info->grid_size = LuaDataUtil::GetVec2(*root, "grid_size");
  info->tile_size = LuaDataUtil::GetVec2(*root, "tile_size", {16, 16});
  info->world_position = LuaDataUtil::GetVec2(*root, "world_position");

  auto tags = root->GetObject("tags");
  if (tags) {
    int tags_count = tags->Count();
    info->tags.reserve(tags_count);
    for (int i = 1; i <= tags_count; ++i) {
      info->tags.push_back(tags->GetString(i));
    }
  }

  auto tiles = root->GetObject("tiles");
  if (tiles) {
    int tiles_count = tiles->Count();
    info->tiles.reserve(tiles_count);
    for (int i = 1; i <= tiles_count; ++i) {
      info->tiles.push_back(GetTile(*tiles, i));
    }
  }

  return info;
}

}  // namespace tilemapeditor