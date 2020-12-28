#include "engine2/sprite_test.h"
#include "engine2/sprite.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {

void SpriteTest::TestCreateStatic() {
  Sprite sprite(/*texture=*/nullptr, Rect<int, 2>{1, 2, 3, 4});

  EXPECT_EQ(1, sprite.FrameCount());

  Sprite::AnimationFrame& frame = sprite.Frame(0);
  EXPECT_EQ(&frame, &(sprite.CurrentFrame()));

  EXPECT_EQ(1, frame.source_rect.x());
  EXPECT_EQ(2, frame.source_rect.y());
  EXPECT_EQ(3, frame.source_rect.w());
  EXPECT_EQ(4, frame.source_rect.h());
  EXPECT_EQ(0, frame.dest_offset.x());
  EXPECT_EQ(0, frame.dest_offset.y());
  EXPECT_EQ(0, frame.duration.ToMicroseconds());
}

SpriteTest::SpriteTest()
    : TestGroup("SpriteTest",
                {
                    std::bind(&SpriteTest::TestCreateStatic, this),
                }) {}

}  // namespace test
}  // namespace engine2