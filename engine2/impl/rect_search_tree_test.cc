#include "engine2/impl/rect_search_tree_test.h"
#include "engine2/impl/rect_search_tree.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {
namespace {

class SomeObject : public RectSearchTree<SomeObject>::RectObject {
 public:
  SomeObject(Rect<> rect, std::string name) : rect(rect), name(name) {}
  ~SomeObject() = default;

  const SomeObject* on_touch_rep = nullptr;
  const SomeObject* on_overlap_rep = nullptr;
  int on_touch_count = 0;
  int on_overlap_count = 0;
  Rect<> rect;
  std::string name;

  Rect<> GetRect() override { return rect; }
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

}  // namespace

#define ASSERT_RECT_EQ(a, b) ASSERT_EQ(RectToString(a), RectToString(b))

void RectSearchTreeTest::TestCreate() {
  Rect<> rect{0, 0, 10, 20};
  auto tree = RectSearchTree<SomeObject>::Create(rect, 0);
  EXPECT_NULL(tree.get());

  tree = RectSearchTree<SomeObject>::Create(rect, 1);
  ASSERT_NOT_NULL(tree.get());
}

void RectSearchTreeTest::TestSingleNode() {
  auto tree = RectSearchTree<SomeObject>::Create({0, 0, 20, 20}, 1);
  ASSERT_NOT_NULL(tree.get());

  SomeObject a({5, 5, 5, 5}, "a");
  tree->Insert(&a);
  SomeObject b({7, 7, 5, 5}, "b");  // overlaps a
  tree->Insert(&b);
  SomeObject c({6, 4, 2, 2}, "c");  // overlaps a but not b
  tree->Insert(&c);
  SomeObject d({6, 3, 1, 1}, "d");  // touches c
  tree->Insert(&d);

  tree->RunCallbacksOn(&a);
  EXPECT_EQ(2, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(0, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(0, c.on_overlap_count);
  EXPECT_EQ(0, c.on_touch_count);
  EXPECT_EQ(0, d.on_overlap_count);
  EXPECT_EQ(0, d.on_touch_count);

  tree->RunCallbacksOn(&b);
  EXPECT_EQ(2, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(1, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(0, c.on_overlap_count);
  EXPECT_EQ(0, c.on_touch_count);
  EXPECT_EQ(0, d.on_overlap_count);
  EXPECT_EQ(0, d.on_touch_count);
  EXPECT_EQ(&a, b.on_overlap_rep);

  tree->RunCallbacksOn(&c);
  EXPECT_EQ(2, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(1, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(1, c.on_overlap_count);
  EXPECT_EQ(1, c.on_touch_count);
  EXPECT_EQ(0, d.on_overlap_count);
  EXPECT_EQ(0, d.on_touch_count);
  EXPECT_EQ(&a, c.on_overlap_rep);
  EXPECT_EQ(&d, c.on_touch_rep);

  tree->RunCallbacksOn(&d);
  EXPECT_EQ(2, a.on_overlap_count);
  EXPECT_EQ(0, a.on_touch_count);
  EXPECT_EQ(1, b.on_overlap_count);
  EXPECT_EQ(0, b.on_touch_count);
  EXPECT_EQ(1, c.on_overlap_count);
  EXPECT_EQ(1, c.on_touch_count);
  EXPECT_EQ(0, d.on_overlap_count);
  EXPECT_EQ(1, d.on_touch_count);
  EXPECT_EQ(&c, d.on_touch_rep);
}

void RectSearchTreeTest::TestHeight2() {
  auto tree = RectSearchTree<SomeObject>::Create({0, 0, 100, 100}, 2);
  SomeObject map({0, 0, 100, 100}, "map");
  tree->Insert(&map);
  SomeObject a({25, 25, 5, 5}, "a");
  tree->Insert(&a);
  SomeObject b({45, 45, 5, 5}, "b");
  tree->Insert(&b);
  SomeObject c({50, 45, 3, 3}, "c");
  tree->Insert(&c);
  SomeObject d({45, 50, 3, 3}, "d");
  tree->Insert(&d);

  tree->RunCallbacksOn(&map);
  EXPECT_EQ(4, map.on_overlap_count);

  tree->RunCallbacksOn(&a);
  EXPECT_EQ(1, a.on_overlap_count);
  EXPECT_EQ(&map, a.on_overlap_rep);

  tree->RunCallbacksOn(&b);
  EXPECT_EQ(1, b.on_overlap_count);
  EXPECT_EQ(&map, b.on_overlap_rep);
  EXPECT_EQ(2, b.on_touch_count);

  tree->RunCallbacksOn(&c);
  EXPECT_EQ(1, c.on_touch_count);
  EXPECT_EQ(&b, c.on_touch_rep);

  tree->RunCallbacksOn(&d);
  EXPECT_EQ(1, d.on_touch_count);
  EXPECT_EQ(&b, d.on_touch_rep);
}

void RectSearchTreeTest::TestFindOutsideBounds() {
  auto tree = RectSearchTree<SomeObject>::Create({0, 0, 10, 10}, 1);
  SomeObject a({-1, -1, 2, 2}, "a");

  ASSERT_NOT_NULL(tree->InsertTrimmed(&a));

  SomeObject b({-2, -1, 1, 1}, "b");
  ASSERT_NOT_NULL(tree->InsertTrimmed(&b));

  // Don't run callbacks outside the tree?
  tree->RunCallbacksOn(&b);
  EXPECT_EQ(0, b.on_touch_count);
  tree->RunCallbacksOn(&a);
  EXPECT_EQ(1, a.on_touch_count);
}

RectSearchTreeTest::RectSearchTreeTest()
    : TestGroup("RectSearchTreeTest",
                {
                    std::bind(&RectSearchTreeTest::TestCreate, this),
                    std::bind(&RectSearchTreeTest::TestSingleNode, this),
                    std::bind(&RectSearchTreeTest::TestHeight2, this),
                    std::bind(&RectSearchTreeTest::TestFindOutsideBounds, this),
                }) {}

}  // namespace test
}  // namespace engine2