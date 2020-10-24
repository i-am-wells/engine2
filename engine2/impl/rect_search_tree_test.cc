#include "engine2/impl/rect_search_tree_test.h"
#include "engine2/impl/rect_search_tree.h"
#include "engine2/rect_object.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {
namespace {

template <int N = 2>
class SomeObject
    : public RectObject<int64_t, N>,
      public RectSearchTree<SomeObject<N>*, N>::OverlapAndTouchReceiver {
 public:
  SomeObject(Rect<int64_t, N> rect, std::string name)
      : rect(rect), name(name) {}
  ~SomeObject() = default;

  const SomeObject* on_touch_rep = nullptr;
  const SomeObject* on_overlap_rep = nullptr;
  int on_touch_count = 0;
  int on_overlap_count = 0;
  Rect<int64_t, N> rect;
  std::string name;

  // RectObject
  Rect<int64_t, N> GetRect() override { return rect; }

  // OverlapAndTouchReceiver
  void OnTouch(SomeObject* other) override {
    on_touch_rep = other;
    ++on_touch_count;
  }
  void OnOverlap(SomeObject* other) override {
    on_overlap_rep = other;
    ++on_overlap_count;
  }
};

std::string RectToString(const Rect<>& rect) {
  return BuildString("{", rect.x(), ", ", rect.y(), ", ", rect.w(), ", ",
                     rect.h(), "}");
}

template <int N>
void RunCallbacksOn(RectSearchTree<SomeObject<N>*, N>* tree,
                    SomeObject<N>* receiver) {
  tree->FindOverlapsAndTouches(receiver->GetRect(), true, true, receiver);
}

}  // namespace

#define ASSERT_RECT_EQ(a, b) ASSERT_EQ(RectToString(a), RectToString(b))

void RectSearchTreeTest::TestCreate() {
  Rect<> rect{0, 0, 10, 20};
  auto tree = RectSearchTree<SomeObject<>*>::Create(rect, 0);
  EXPECT_NULL(tree.get());

  tree = RectSearchTree<SomeObject<>*>::Create(rect, 1);
  ASSERT_NOT_NULL(tree.get());
}

void RectSearchTreeTest::TestSingleNode() {
  auto tree = RectSearchTree<SomeObject<>*>::Create({0, 0, 20, 20}, 1);
  ASSERT_NOT_NULL(tree.get());

  SomeObject<> a({5, 5, 5, 5}, "a");
  tree->Insert(&a);
  SomeObject<> b({7, 7, 5, 5}, "b");  // overlaps a
  tree->Insert(&b);
  SomeObject<> c({6, 4, 2, 2}, "c");  // overlaps a but not b
  tree->Insert(&c);
  SomeObject<> d({6, 3, 1, 1}, "d");  // touches c
  tree->Insert(&d);

  RunCallbacksOn(tree.get(), &a);
  EXPECT_EQ(3, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(0, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(0, c.on_overlap_count);
  EXPECT_EQ(0, c.on_touch_count);
  EXPECT_EQ(0, d.on_overlap_count);
  EXPECT_EQ(0, d.on_touch_count);

  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(3, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(2, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(0, c.on_overlap_count);
  EXPECT_EQ(0, c.on_touch_count);
  EXPECT_EQ(0, d.on_overlap_count);
  EXPECT_EQ(0, d.on_touch_count);

  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(3, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(2, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(2, c.on_overlap_count);
  EXPECT_EQ(1, c.on_touch_count);
  EXPECT_EQ(0, d.on_overlap_count);
  EXPECT_EQ(0, d.on_touch_count);

  RunCallbacksOn(tree.get(), &d);
  EXPECT_EQ(3, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(2, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(2, c.on_overlap_count);
  EXPECT_EQ(1, c.on_touch_count);
  EXPECT_EQ(1, d.on_overlap_count);
  EXPECT_EQ(1, d.on_touch_count);
}

void RectSearchTreeTest::TestHeight2() {
  auto tree = RectSearchTree<SomeObject<>*>::Create({0, 0, 100, 100}, 2);
  SomeObject<> map({0, 0, 100, 100}, "map");
  tree->Insert(&map);
  SomeObject<> a({25, 25, 5, 5}, "a");
  tree->Insert(&a);
  SomeObject<> b({45, 45, 5, 5}, "b");
  tree->Insert(&b);
  SomeObject<> c({50, 45, 3, 3}, "c");
  tree->Insert(&c);
  SomeObject<> d({45, 50, 3, 3}, "d");
  tree->Insert(&d);

  RunCallbacksOn(tree.get(), &map);
  EXPECT_EQ(5, map.on_overlap_count);

  RunCallbacksOn(tree.get(), &a);
  EXPECT_EQ(2, a.on_overlap_count);

  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(2, b.on_overlap_count);
  EXPECT_EQ(2, b.on_touch_count);

  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(1, c.on_touch_count);

  RunCallbacksOn(tree.get(), &d);
  EXPECT_EQ(1, d.on_touch_count);
}

void RectSearchTreeTest::TestFindOutsideBounds() {
  auto tree = RectSearchTree<SomeObject<>*>::Create({0, 0, 10, 10}, 1);
  SomeObject<> a({-1, -1, 2, 2}, "a");

  ASSERT_NOT_NULL(tree->InsertTrimmed(&a));

  SomeObject<> b({-2, -1, 1, 1}, "b");
  ASSERT_NOT_NULL(tree->InsertTrimmed(&b));

  // Don't run callbacks outside the tree?
  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(0, b.on_touch_count);
  RunCallbacksOn(tree.get(), &a);
  EXPECT_EQ(1, a.on_touch_count);
}

void RectSearchTreeTest::Test4D() {
  auto tree = RectSearchTree<SomeObject<4>*, 4>::Create(
      {0, 0, 0, 0, 100, 100, 100, 100}, 8);

  SomeObject<4> a({48, 48, 48, 48, 2, 2, 2, 2}, "a");
  tree->Insert(&a);

  SomeObject<4> b({50, 50, 50, 50, 2, 2, 2, 2}, "b");
  tree->Insert(&b);

  RunCallbacksOn(tree.get(), &a);
  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(1, a.on_touch_count);
  EXPECT_EQ(1, b.on_touch_count);

  SomeObject<4> c({52, 52, 52, 52, 2, 2, 2, 2}, "c");
  tree->Insert(&c);
  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(1, c.on_touch_count);

  SomeObject<4> d({52, 52, 52, 53, 100, 100, 100, 100}, "d");
  tree->InsertTrimmed(&d);
  RunCallbacksOn(tree.get(), &d);
  EXPECT_EQ(2, d.on_overlap_count);
  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(3, c.on_overlap_count);
}

RectSearchTreeTest::RectSearchTreeTest()
    : TestGroup("RectSearchTreeTest",
                {
                    std::bind(&RectSearchTreeTest::TestCreate, this),
                    std::bind(&RectSearchTreeTest::TestSingleNode, this),
                    std::bind(&RectSearchTreeTest::TestHeight2, this),
                    std::bind(&RectSearchTreeTest::TestFindOutsideBounds, this),
                    std::bind(&RectSearchTreeTest::Test4D, this),
                }) {}

}  // namespace test
}  // namespace engine2