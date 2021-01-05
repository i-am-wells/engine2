#include "engine2/init.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

namespace engine2 {

bool Init() {
  int img_init_flags = IMG_INIT_PNG | IMG_INIT_JPG;
  int mix_init_flags = 0;

  return (SDL_Init(SDL_INIT_AUDIO) == 0) &&
         (IMG_Init(img_init_flags) == img_init_flags) &&
         (Mix_Init(mix_init_flags) == mix_init_flags) && (TTF_Init() == 0);
}

void Quit() {
  TTF_Quit();
  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
}

}  // namespace engine2