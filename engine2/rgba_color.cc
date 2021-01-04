#include "engine2/rgba_color.h"

namespace engine2 {

SDL_Color RgbaColor::ToSDLColor() const {
  return {r, g, b, a};
}

}  // namespace engine2