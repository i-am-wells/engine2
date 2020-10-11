#include "engine2/texture.h"

#include <SDL2/SDL_image.h>
#include <string>

#include "engine2/graphics2d.h"

namespace engine2 {

// static
std::unique_ptr<Texture> Texture::Create(const Graphics2D& renderer,
                                         uint32_t sdl_pixelformat,
                                         int sdl_textureaccess,
                                         int width,
                                         int height) {
  SDL_Texture* sdl_texture = SDL_CreateTexture(
      renderer.renderer(), sdl_pixelformat, sdl_textureaccess, width, height);
  if (!sdl_texture)
    return nullptr;
  return std::make_unique<Texture>(sdl_texture);
}

// static
std::unique_ptr<Texture> Texture::LoadFromImage(const Graphics2D& renderer,
                                                const std::string& path) {
  SDL_Surface* surface = IMG_Load(path.c_str());
  if (!surface)
    return nullptr;

  SDL_Texture* sdl_texture =
      SDL_CreateTextureFromSurface(renderer.renderer(), surface);
  SDL_FreeSurface(surface);
  if (!sdl_texture)
    return nullptr;
  return std::make_unique<Texture>(sdl_texture);
}

Texture::Texture(SDL_Texture* sdl_texture) : texture_(sdl_texture) {}

Texture::~Texture() {
  if (texture_)
    SDL_DestroyTexture(texture_);
}

bool Texture::SetColorMod(RgbaColor color) {
  return SDL_SetTextureColorMod(texture_, color.r, color.g, color.b) >= 0;
}

bool Texture::SetAlphaMod(uint8_t alpha) {
  return SDL_SetTextureAlphaMod(texture_, alpha) >= 0;
}

Rect<> Texture::GetSize() {
  int w, h;
  if (SDL_QueryTexture(texture_, /*format=*/nullptr, /*access=*/nullptr, &w,
                       &h) < 0) {
    return {0, 0, 0, 0};
  }
  return {0, 0, w, h};
}

}  // namespace engine2