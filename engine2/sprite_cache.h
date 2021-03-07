#ifndef ENGINE2_SPRITE_CACHE_H_
#define ENGINE2_SPRITE_CACHE_H_

#include <map>
#include <optional>
#include <string>

#include "engine2/sprite.h"
#include "engine2/texture_cache.h"

namespace engine2 {

class SpriteCache {
 public:
  SpriteCache(TextureCache* texture_cache);

  Sprite* Get(const std::string& path);
  void Put(const std::string& path, Sprite sprite);
  std::optional<std::string> LookupName(Sprite* sprite) const;

 private:
  TextureCache* texture_cache_;
  std::map<std::string, Sprite> map_;
};

}  // namespace engine2

#endif  // ENGINE2_SPRITE_CACHE_H_