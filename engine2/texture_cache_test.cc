#include "engine2/texture_cache_test.h"
#include "engine2/impl/basic_graphics2d.h"
#include "engine2/test/assert_macros.h"
#include "engine2/texture_cache.h"

#include <SDL2/SDL_video.h>

namespace engine2 {
namespace test {

constexpr char kImageThatExists[] = "engine2/test/data/pirate0.png";
constexpr char kImageThatDoesntExist[] = "asdfasdfasdf.png";

void TextureCacheTest::TestGet() {
  TextureCache cache(graphics_.get());
  Texture* texture = cache.Get(kImageThatExists);
  ASSERT_NOT_NULL(texture);

  Rect<> size = texture->GetSize();
  EXPECT_EQ(16, size.w());
  EXPECT_EQ(32, size.h());

  Texture* texture2 = cache.Get(kImageThatExists);
  EXPECT_EQ(texture, texture2);
}

void TextureCacheTest::TestGetNonexistant() {
  TextureCache cache(graphics_.get());
  ASSERT_NULL(cache.Get(kImageThatDoesntExist));
}

TextureCacheTest::TextureCacheTest()
    : TestGroup("TextureCacheTest",
                {
                    std::bind(&TextureCacheTest::TestGet, this),
                    std::bind(&TextureCacheTest::TestGetNonexistant, this),
                }) {
  if (SDL_VideoInit(nullptr) != 0) {
    Log("SDL_VideoInit failed: ", SDL_GetError());
    return;
  }

  window_ = Window::Create("TextureCacheTest", {0, 0, 100, 100}, 0);
  if (!window_) {
    Log("Failed to create window: ", SDL_GetError());
    return;
  }

  graphics_ = BasicGraphics2D::Create(*window_, 0);
  if (!graphics_) {
    Log("Failed to create renderer: ", SDL_GetError());
    return;
  }
}

TextureCacheTest::~TextureCacheTest() {
  SDL_VideoQuit();
}

}  // namespace test
}  // namespace engine2