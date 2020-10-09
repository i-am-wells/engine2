#include "engine2/texture_cache.h"

namespace engine2 {

TextureCache::TextureCache(Graphics2D* graphics) : graphics_(graphics) {}

void TextureCache::Clear() {
  textures_.clear();
}

Texture* TextureCache::Get(const std::string& path) {
  auto iterator = textures_.find(path);
  if (iterator != textures_.end())
    return iterator->second.get();

  auto texture = Texture::LoadFromImage(*graphics_, path);
  Texture* result = texture.get();
  textures_.insert(iterator, {path, std::move(texture)});
  return result;
}

}  // namespace engine2