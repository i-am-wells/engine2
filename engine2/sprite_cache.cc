#include <optional>

#include "engine2/sprite_cache.h"
#include "luadata/luadata.h"

namespace engine2 {
namespace {

using luadata::LuaData;

Vec<int64_t, 2> ReadVec2(const LuaData& obj) {
  return {obj.GetInt("x"), obj.GetInt("y")};
}

Rect<int64_t, 2> ReadRect(const LuaData& obj) {
  return {obj.GetInt("x"), obj.GetInt("y"), obj.GetInt("w"), obj.GetInt("h")};
}

std::string GetSpriteSheetFileName(const std::string& path) {
  return path.substr(0, path.find(':'));
}

}  // namespace

SpriteCache::SpriteCache(TextureCache* texture_cache)
    : texture_cache_(texture_cache) {}

Sprite* SpriteCache::Get(const std::string& path) {
  auto iter = map_.find(path);
  if (iter == map_.end())
    return LoadInternal(path);

  return &(iter->second);
}

void SpriteCache::Put(const std::string& name, Sprite sprite) {
  map_.insert_or_assign(name, sprite);
}

Sprite* SpriteCache::Create(const std::string& path,
                            Texture* texture,
                            int frame_count) {
  auto [iter, did_insert] = map_.emplace(path, Sprite(texture, frame_count));
  return &(iter->second);
}

std::optional<std::string> SpriteCache::LookupName(Sprite* sprite) const {
  for (auto& named_sprite : map_) {
    if (&(named_sprite.second) == sprite)
      return named_sprite.first;
  }
  return std::nullopt;
}

Sprite* SpriteCache::LoadInternal(const std::string& path) {
  size_t colon_pos = path.find(':');
  std::string file_path = path.substr(0, colon_pos);
  std::string sprite_wanted_name{};
  if (colon_pos != std::string::npos)
    sprite_wanted_name = path.substr(colon_pos + 1);

  LuaData::LoadResult load_result = LuaData::LoadFile(file_path);
  if (!load_result.data)
    return nullptr;

  Texture* texture = texture_cache_->Get(load_result.data->GetString("image"));
  if (!texture)
    return nullptr;

  Sprite* result = nullptr;

  LuaData sprites = load_result.data->GetObject("sprites");
  int sprites_count = sprites.Count();
  for (int i = 1; i <= sprites_count; ++i) {
    LuaData sprite_data = sprites.GetObject(i);

    // Name can be empty.
    std::string name = sprite_data.GetString("name");

    LuaData frames = sprite_data.GetObject("frames");
    int frame_count = frames.Count();
    if (frame_count < 1)
      continue;

    std::string new_sprite_name = file_path;
    if (!name.empty())
      new_sprite_name += ':' + name;

    Sprite* sprite = Create(new_sprite_name, texture, frame_count);
    if (name == sprite_wanted_name)
      result = sprite;

    for (int frame_idx = 1; frame_idx <= frame_count; ++frame_idx) {
      LuaData frame = frames.GetObject(frame_idx);

      sprite->AddFrame(
          {ReadRect(frame.GetObject("source_rect")),
           ReadVec2(frame.GetObject("dest_offset")),
           Time::Delta::FromMicroseconds(frame.GetInt("duration_ms") * 1000)});
    }
  }

  return result;
}

}  // namespace engine2