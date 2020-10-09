#ifndef ENGINE2_TEXTURE_CACHE_H_
#define ENGINE2_TEXTURE_CACHE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "engine2/texture.h"

namespace engine2 {

class TextureCache {
 public:
  TextureCache(Graphics2D* graphics);
  void SetGraphics(Graphics2D* graphics) { graphics_ = graphics; }

  void Clear();
  Texture* Get(const std::string& path);

 private:
  Graphics2D* graphics_;
  std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
};

}  // namespace engine2

#endif  // ENGINE2_TEXTURE_CACHE_H_