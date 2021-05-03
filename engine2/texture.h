#ifndef ENGINE2_TEXTURE_H_
#define ENGINE2_TEXTURE_H_

#include <SDL2/SDL_render.h>

#include <cstdint>
#include <memory>

#include "engine2/rect.h"
#include "engine2/rgba_color.h"

namespace engine2 {

class Graphics2D;
class BasicGraphics2D;

class Texture {
 public:
  static std::unique_ptr<Texture> Create(const Graphics2D& renderer,
                                         uint32_t sdl_pixelformat,
                                         int sdl_textureaccess,
                                         int width,
                                         int height);
  static std::unique_ptr<Texture> LoadFromImage(const Graphics2D& renderer,
                                                const std::string& path);
  // If you use this constructor, don't destroy sdl_texture yourself; ~Texture()
  // does that.
  explicit Texture(SDL_Texture* sdl_texture);

  ~Texture();

  bool SetColorMod(RgbaColor color);
  bool SetAlphaMod(uint8_t alpha);
  Rect<> GetSize() const;

 private:
  friend class BasicGraphics2D;
  SDL_Texture* texture_ = nullptr;
};

}  // namespace engine2

#endif  // ENGINE2_TEXTURE_H_