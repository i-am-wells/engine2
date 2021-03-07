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

std::optional<Sprite> LoadSprite(const std::string& path,
                                 TextureCache* texture_cache) {
  auto [data, error] = LuaData::LoadFile(path);
  if (!data)
    return std::nullopt;

  Texture* texture = texture_cache->Get(data->GetString("image"));
  if (!texture)
    return std::nullopt;

  LuaData frames = data->GetObject("frames");
  int frame_count = frames.Count();
  if (frame_count < 1)
    return std::nullopt;

  Sprite result(texture, frame_count);

  for (int i = 1; i <= frame_count; ++i) {
    LuaData frame = frames.GetObject(i);

    result.AddFrame(
        {ReadRect(frame.GetObject("source_rect")),
         ReadVec2(frame.GetObject("dest_offset")),
         Time::Delta::FromMicroseconds(frame.GetInt("duration_ms") * 1000)});
  }
  return result;
}

}  // namespace

SpriteCache::SpriteCache(TextureCache* texture_cache)
    : texture_cache_(texture_cache) {}

Sprite* SpriteCache::Get(const std::string& path) {
  auto iter = map_.find(path);
  if (iter == map_.end()) {
    auto maybe_sprite = LoadSprite(path, texture_cache_);
    if (maybe_sprite.has_value())
      iter = map_.insert({path, maybe_sprite.value()}).first;
    else
      return nullptr;
  }

  return &(iter->second);
}

void SpriteCache::Put(const std::string& name, Sprite sprite) {
  map_.insert_or_assign(name, sprite);
}

std::optional<std::string> SpriteCache::LookupName(Sprite* sprite) const {
  for (auto& named_sprite : map_) {
    if (&(named_sprite.second) == sprite)
      return named_sprite.first;
  }
  return std::nullopt;
}

}  // namespace engine2