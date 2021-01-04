#include "engine2/font.h"

namespace engine2 {

// static
std::unique_ptr<Font> Font::Load(const std::string& ttf_file_path,
                                 int point_size) {
  TTF_Font* font = TTF_OpenFont(ttf_file_path.c_str(), point_size);
  if (!font)
    return nullptr;
  return std::unique_ptr<Font>(new Font(font));
}

Font::Font(TTF_Font* font) : font_(font) {}

Font::~Font() {
  TTF_CloseFont(font_);
}

std::unique_ptr<Texture> Font::Render(Graphics2D* graphics,
                                      const std::string& text,
                                      RgbaColor color) {
  SDL_Surface* surface =
      TTF_RenderUTF8_Solid(font_, text.c_str(), color.ToSDLColor());
  if (!surface)
    return nullptr;

  SDL_Texture* sdl_texture =
      SDL_CreateTextureFromSurface(graphics->renderer(), surface);
  SDL_FreeSurface(surface);
  if (!sdl_texture)
    return nullptr;

  return std::unique_ptr<Texture>(new Texture(sdl_texture));
}

}  // namespace engine2