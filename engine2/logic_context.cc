#include "engine2/logic_context.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "engine2/impl/logic_context_impl.h"

namespace engine2 {

// static
std::unique_ptr<LogicContext> LogicContext::Create() {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    return nullptr;
  }

  int img_init_flags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (IMG_Init(img_init_flags) != img_init_flags) {
    return nullptr;
  }

  int mix_init_flags = 0;
  if (Mix_Init(mix_init_flags) != mix_init_flags) {
    return nullptr;
  }

  return std::make_unique<LogicContextImpl>();
}

}  // namespace engine2