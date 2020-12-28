#include "engine2/sprite_test.h"
#include "engine2/sprite.h"
#include "engine2/test/assert_macros.h"
#include "engine2/test_clock.h"
#include "engine2/test_graphics2d.h"

namespace engine2 {
namespace test {

void SpriteTest::TestCreateStatic() {
  Sprite sprite(/*texture=*/nullptr, Rect<int64_t, 2>{1, 2, 3, 4});

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

void SpriteTest::TestDraw() {
  Sprite sprite(/*texture=*/nullptr, Rect<int64_t, 2>{5, 6, 7, 8});
  TestGraphics2D graphics;
  sprite.Draw(&graphics, {1, 2});
  auto dest = Rect<int64_t, 2>{1, 2, 7, 8};
  EXPECT_TRUE(graphics.draw_texture_dest == dest);
}

void SpriteTest::TestOffsetDraw() {
  Sprite sprite(/*texture=*/nullptr,
                /*source_rect=*/Rect<int64_t, 2>{5, 6, 7, 8},
                /*dest_offset=*/Point<int64_t, 2>{20, 20});
  TestGraphics2D graphics;
  sprite.Draw(&graphics, {1, 2});
  auto dest = Rect<int64_t, 2>{21, 22, 7, 8};
  EXPECT_TRUE(graphics.draw_texture_dest == dest);
}

void SpriteTest::TestUpdate() {
  // Freeze time
  Time start_time = Time::Now();
  TestClock test_clock(start_time);

  Sprite sprite(/*texture=*/nullptr,
                /*source_rect=*/Rect<int64_t, 2>{5, 6, 7, 8},
                /*dest_offset=*/Point<int64_t, 2>{20, 20},
                /*duration=*/Time::Delta::FromSeconds(1));
  sprite.AddFrame({Rect<int64_t, 2>{9, 10, 11, 12}, Point<int64_t, 2>{},
                   Time::Delta::FromSeconds(2)});

  EXPECT_EQ(&(sprite.Frame(0)), &(sprite.CurrentFrame()));

  sprite.Update(start_time + Time::Delta::FromSeconds(0.999));
  EXPECT_EQ(&(sprite.Frame(0)), &(sprite.CurrentFrame()));

  sprite.Update(start_time + Time::Delta::FromSeconds(1));
  EXPECT_EQ(&(sprite.Frame(1)), &(sprite.CurrentFrame()));

  sprite.Update(start_time + Time::Delta::FromSeconds(2.5));
  EXPECT_EQ(&(sprite.Frame(1)), &(sprite.CurrentFrame()));

  sprite.Update(start_time + Time::Delta::FromSeconds(2.5));
  EXPECT_EQ(&(sprite.Frame(1)), &(sprite.CurrentFrame()));

  // Backwards updates do nothing.
  sprite.Update(start_time - Time::Delta::FromSeconds(2.5));
  EXPECT_EQ(&(sprite.Frame(1)), &(sprite.CurrentFrame()));
  sprite.Update(start_time - Time::Delta::FromSeconds(3));
  EXPECT_EQ(&(sprite.Frame(1)), &(sprite.CurrentFrame()));

  sprite.Update(start_time - Time::Delta::FromSeconds(3 * 1000000 + 2.1));
  EXPECT_EQ(&(sprite.Frame(1)), &(sprite.CurrentFrame()));
}

SpriteTest::SpriteTest()
    : TestGroup("SpriteTest",
                {
                    std::bind(&SpriteTest::TestCreateStatic, this),
                    std::bind(&SpriteTest::TestDraw, this),
                    std::bind(&SpriteTest::TestOffsetDraw, this),
                    std::bind(&SpriteTest::TestUpdate, this),
                }) {}

}  // namespace test
}  // namespace engine2