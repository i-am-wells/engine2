#ifndef ENGINE2_RGBACOLOR_H_
#define ENGINE2_RGBACOLOR_H_

#include <SDL2/SDL_pixels.h>

#include <cstdint>

namespace engine2 {

inline constexpr uint8_t kOpaque = SDL_ALPHA_OPAQUE;
inline constexpr uint8_t kTransparent = SDL_ALPHA_TRANSPARENT;

struct RgbaColor {
  uint8_t r, g, b, a;
  SDL_Color ToSDLColor() const;
};

// TODO more colors as needed
inline constexpr RgbaColor kBlack{0, 0, 0, kOpaque};
inline constexpr RgbaColor kGray{128, 128, 128, kOpaque};
inline constexpr RgbaColor kDarkGray{64, 64, 64, kOpaque};
inline constexpr RgbaColor kWhite{255, 255, 255, kOpaque};
inline constexpr RgbaColor kRed{255, 0, 0, kOpaque};
inline constexpr RgbaColor kGreen{0, 255, 0, kOpaque};

}  // namespace engine2

#endif  // ENGINE2_RGBACOLOR_H_