#include "engine2/sprite_cache_test.h"
#include "engine2/sprite_cache.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {

void SpriteCacheTest::TestLookupName() {
  SpriteCache cache(/*texture_cache=*/nullptr);

  const std::string kName("sprite0");
  cache.Put(kName, Sprite(nullptr));
  Sprite* got = cache.Get(kName);
  ASSERT_NOT_NULL(got);

  auto maybe_name = cache.LookupName(got);
  ASSERT_TRUE(maybe_name.has_value());
  EXPECT_EQ(kName, maybe_name.value());
}

SpriteCacheTest::SpriteCacheTest()
    : TestGroup("SpriteCacheTest",
                {
                    {std::bind(&SpriteCacheTest::TestLookupName, this)},
                }) {}

}  // namespace test
}  // namespace engine2