#ifndef ENGINE2_FONT_H_
#define ENGINE2_FONT_H_

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

#include "engine2/graphics2d.h"
#include "engine2/rgba_color.h"
#include "engine2/texture.h"

namespace engine2 {

class Font {
 public:
  static std::unique_ptr<Font> Load(const std::string& ttf_file_path,
                                    int point_size);

  ~Font();

  std::unique_ptr<Texture> Render(Graphics2D* graphics,
                                  const std::string& text,
                                  RgbaColor color);

  TTF_Font* font() { return font_; }

 private:
  Font(TTF_Font* font);

  TTF_Font* font_;
};

}  // namespace engine2

#endif  // ENGINE2_FONT_H_