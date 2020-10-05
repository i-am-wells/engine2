#ifndef ENGINE2_RGBACOLOR_H_
#define ENGINE2_RGBACOLOR_H_

#include <SDL2/SDL_pixels.h>

#include <cstdint>

namespace engine2 {

static constexpr uint8_t kOpaque = SDL_ALPHA_OPAQUE;
static constexpr uint8_t kTransparent = SDL_ALPHA_TRANSPARENT;

struct RgbaColor {
  uint8_t r, g, b, a;
};

}  // namespace engine2

#endif  // ENGINE2_RGBACOLOR_H_