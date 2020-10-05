#include "engine2/lua/engine2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

namespace engine2 {

int luaopen_engine2(lua_State* L) {
  // TODO: leave video init for later
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    return 0;
  }

  int img_init_flags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (IMG_Init(img_init_flags) != img_init_flags) {
    return 0;
  }

  int mix_init_flags = 0;
  if (Mix_Init(mix_init_flags) != mix_init_flags) {
    return 0;
  }

  // TODO add functions here
  const luaL_Reg engine2_functions[] = {{nullptr, nullptr}};
  luaL_newlib(L, engine2_functions);
  // TODO set other fields here
  return 1;
}

}  // namespace engine2