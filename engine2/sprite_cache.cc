#include <optional>

#include "engine2/data.h"
#include "engine2/luadata_util.h"
#include "engine2/sprite_cache.h"
#include "luadata/object.h"
#include "luadata/root_object.h"

namespace engine2 {
namespace {

using luadata::RootObject;

std::string GetSpriteSheetFileName(const std::string& path) {
  return path.substr(0, path.find(':'));
}

std::string MakeSpriteKey(const std::string& image_path,
                          const std::string& sprite_name) {
  if (sprite_name.empty())
    return image_path;
  return image_path + ":" + sprite_name;
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

bool SpriteCache::LoadSpriteSheet(SpriteSheetInfo info) {
  std::string image_path(info.image_path.data(), info.image_path.size());
  Texture* texture = texture_cache_->Get(image_path);
  if (!texture)
    return false;

  for (SpriteInfo& sprite : info.sprites) {
    std::string sprite_name(sprite.name.data(), sprite.name.size());
    map_.emplace(MakeSpriteKey(image_path, sprite_name),
                 Sprite(texture, std::move(sprite.frames)));
  }
  return true;
}

bool SpriteCache::LoadSpriteSheet(
    const std::string& sprite_sheet_luadata_file) {
  LoadInternal(sprite_sheet_luadata_file);
  return true;
}

Sprite* SpriteCache::LoadInternal(const std::string& path) {
  size_t colon_pos = path.find(':');
  std::string file_path = path.substr(0, colon_pos);
  std::string sprite_wanted_name{};
  if (colon_pos != std::string::npos)
    sprite_wanted_name = path.substr(colon_pos + 1);

  RootObject::LoadResult load_result = RootObject::LoadFile(file_path);
  if (!load_result.data)
    return nullptr;

  std::string image_path = load_result.data->GetString("image");
  Texture* texture = texture_cache_->Get(image_path);
  if (!texture)
    return nullptr;

  Sprite* result = nullptr;

  auto sprites = load_result.data->GetObject("sprites");
  if (!sprites)
    return nullptr;

  int sprites_count = sprites->Count();
  for (int i = 1; i <= sprites_count; ++i) {
    auto sprite_data = sprites->GetObject(i);
    if (!sprite_data)
      return nullptr;

    // Name can be empty.
    std::string name = sprite_data->GetString("name");

    auto frames = sprite_data->GetObject("frames");
    if (!frames)
      return nullptr;
    int frame_count = frames->Count();
    if (frame_count < 1)
      continue;

    std::string new_sprite_name = MakeSpriteKey(image_path, name);

    Sprite* sprite = Create(new_sprite_name, texture, frame_count);
    if (name == sprite_wanted_name)
      result = sprite;

    for (int frame_idx = 1; frame_idx <= frame_count; ++frame_idx) {
      auto frame = frames->GetObject(frame_idx);
      if (!frame)
        return nullptr;

      auto source_rect = frame->GetObject("source_rect");
      auto dest_offset = frame->GetObject("dest_offset");
      if (!source_rect || !dest_offset)
        return nullptr;

      sprite->AddFrame(
          {LuaDataUtil::GetRect2(*frame, "source_rect", {0, 0, 0, 0}),
           LuaDataUtil::GetVec2(*frame, "dest_offset", {0, 0}),
           Time::Delta::FromMicroseconds(frame->GetInt("duration_ms") * 1000)});
    }
  }

  return result;
}

}  // namespace engine2