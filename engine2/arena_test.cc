#include "engine2/arena.h"
#include "engine2/arena_test.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {
namespace {

class SomeActive {
 public:
  SomeActive(Rect<int64_t, 2> rect) : rect(rect) {}
  Rect<int64_t, 2> GetRect() const { return rect; }
  Rect<int64_t, 2> rect;
};

class SomeReactive : public SomeActive {
 public:
  SomeReactive(Rect<int64_t, 2> rect) : SomeActive(rect) {}

  void OnOverlap(SomeActive* other) {
    ++overlap_count;
    overlapping = other;
  }
  void OnTouch(SomeActive* other) {
    ++touch_count;
    touching = other;
  }

  SomeActive* overlapping = nullptr;
  int overlap_count = 0;
  SomeActive* touching = nullptr;
  int touch_count = 0;
};

}  // namespace

void ArenaTest::TestReact() {
  SomeActive active1({10, 10, 5, 5});
  SomeActive active2({20, 10, 5, 5});
  SomeReactive reactive1({10, 15, 5, 5});  // touches active1
  SomeReactive reactive2({12, 10, 8, 5});  // overlaps active1, touches active2

  Arena<SomeActive, SomeReactive, 2> arena({0, 0, 40, 40}, 1);
  arena.AddActive(&active1);
  arena.AddActive(&active2);
  arena.AddReactive(&reactive1);
  arena.AddReactive(&reactive2);

  arena.ReactAll();

  EXPECT_EQ(0, reactive1.overlap_count);
  EXPECT_EQ(1, reactive1.touch_count);
  EXPECT_EQ(&active1, reactive1.touching);

  EXPECT_EQ(1, reactive2.overlap_count);
  EXPECT_EQ(&active1, reactive2.overlapping);
  EXPECT_EQ(1, reactive2.touch_count);
  EXPECT_EQ(&active2, reactive2.touching);
}

void ArenaTest::TestMoveActive() {
  Arena<SomeActive, SomeReactive, 2> arena({0, 0, 10, 10}, 2);
  SomeActive active({2, 2, 2, 2});
  arena.AddActive(&active);
  SomeReactive reactive({7, 7, 2, 2});
  arena.AddReactive(&reactive);

  arena.ReactAll();
  EXPECT_EQ(0, reactive.overlap_count);

  active.rect.pos = {{6, 6}};
  arena.ReactAll();
  EXPECT_EQ(0, reactive.overlap_count);

  arena.Update(&active);
  arena.ReactAll();
  EXPECT_EQ(1, reactive.overlap_count);
}

ArenaTest::ArenaTest()
    : TestGroup("ArenaTest",
                {
                    std::bind(&ArenaTest::TestReact, this),
                    std::bind(&ArenaTest::TestMoveActive, this),
                }) {}

}  // namespace test
}  // namespace engine2