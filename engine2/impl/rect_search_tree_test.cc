#include "engine2/impl/rect_search_tree_test.h"
#include "engine2/impl/rect_search_tree.h"
#include "engine2/rect_object.h"
#include "engine2/test/assert_macros.h"

#include <unordered_set>

namespace engine2 {
namespace test {
namespace {

template <int N = 2>
class SomeObject {
 public:
  SomeObject(Rect<int64_t, N> rect, std::string name)
      : rect(rect), name(name) {}
  ~SomeObject() = default;

  std::unordered_set<SomeObject*> overlap_set;
  std::unordered_set<SomeObject*> touch_set;

  Rect<int64_t, N> rect;
  std::string name;

  void OnTouch(SomeObject* other) { touch_set.insert(other); }
  void OnOverlap(SomeObject* other) { overlap_set.insert(other); }
};

std::string RectToString(const Rect<>& rect) {
  return BuildString("{", rect.x(), ", ", rect.y(), ", ", rect.w(), ", ",
                     rect.h(), "}");
}

template <int N>
void RunCallbacksOn(RectSearchTree<N, SomeObject<N>*>* tree,
                    SomeObject<N>* receiver) {
  for (SomeObject<N>* object : tree->Near(receiver->rect)) {
    if (object->rect.Overlaps(receiver->rect))
      receiver->OnOverlap(object);

    if (object->rect.Touches(receiver->rect))
      receiver->OnTouch(object);
  }
}

}  // namespace

#define ASSERT_RECT_EQ(a, b) ASSERT_EQ(RectToString(a), RectToString(b))
#define ASSERT_ITER(iter) ASSERT_TRUE(!!iter)

void RectSearchTreeTest::TestCreate() {
  Rect<> rect{0, 0, 10, 20};
  auto tree = RectSearchTree<2, SomeObject<>*>::Create(rect, 0);
  EXPECT_NULL(tree.get());

  tree = RectSearchTree<2, SomeObject<>*>::Create(rect, 1);
  ASSERT_NOT_NULL(tree.get());
}

void RectSearchTreeTest::TestSingleNode() {
  auto tree = RectSearchTree<2, SomeObject<>*>::Create({0, 0, 20, 20}, 1);
  ASSERT_NOT_NULL(tree.get());

  SomeObject<> a({5, 5, 5, 5}, "a");
  tree->Insert(a.rect, &a);
  SomeObject<> b({7, 7, 5, 5}, "b");  // overlaps a
  tree->Insert(b.rect, &b);
  SomeObject<> c({6, 4, 2, 2}, "c");  // overlaps a but not b
  tree->Insert(c.rect, &c);
  SomeObject<> d({6, 3, 1, 1}, "d");  // touches c
  tree->Insert(d.rect, &d);

  RunCallbacksOn(tree.get(), &a);
  EXPECT_EQ(3, a.overlap_set.size());
  EXPECT_EQ(1, a.overlap_set.count(&a));
  EXPECT_EQ(1, a.overlap_set.count(&b));
  EXPECT_EQ(1, a.overlap_set.count(&c));
  EXPECT_EQ(0, a.touch_set.size());

  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(2, b.overlap_set.size());
  EXPECT_EQ(1, b.overlap_set.count(&a));
  EXPECT_EQ(1, b.overlap_set.count(&b));
  EXPECT_EQ(0, b.touch_set.size());

  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(2, c.overlap_set.size());
  EXPECT_EQ(1, c.overlap_set.count(&a));
  EXPECT_EQ(1, c.overlap_set.count(&c));
  EXPECT_EQ(1, c.touch_set.size());
  EXPECT_EQ(1, c.touch_set.count(&d));

  RunCallbacksOn(tree.get(), &d);
  EXPECT_EQ(1, d.overlap_set.size());  // d overlaps itself
  EXPECT_EQ(1, d.touch_set.size());
  EXPECT_EQ(1, d.touch_set.count(&c));
}

void RectSearchTreeTest::TestHeight2() {
  auto tree = RectSearchTree<2, SomeObject<>*>::Create({0, 0, 100, 100}, 2);
  SomeObject<> map({0, 0, 100, 100}, "map");
  tree->Insert(map.rect, &map);
  SomeObject<> a({25, 25, 5, 5}, "a");
  tree->Insert(a.rect, &a);
  SomeObject<> b({45, 45, 5, 5}, "b");
  tree->Insert(b.rect, &b);
  SomeObject<> c({50, 45, 3, 3}, "c");
  tree->Insert(c.rect, &c);
  SomeObject<> d({45, 50, 3, 3}, "d");
  tree->Insert(d.rect, &d);

  RunCallbacksOn(tree.get(), &map);
  EXPECT_EQ(5, map.overlap_set.size());
  EXPECT_EQ(1, map.overlap_set.count(&map));
  EXPECT_EQ(1, map.overlap_set.count(&a));
  EXPECT_EQ(1, map.overlap_set.count(&b));
  EXPECT_EQ(1, map.overlap_set.count(&c));
  EXPECT_EQ(1, map.overlap_set.count(&d));

  RunCallbacksOn(tree.get(), &a);
  EXPECT_EQ(2, a.overlap_set.size());
  EXPECT_EQ(1, a.overlap_set.count(&a));
  EXPECT_EQ(1, a.overlap_set.count(&map));

  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(2, b.overlap_set.size());
  EXPECT_EQ(1, b.overlap_set.count(&b));
  EXPECT_EQ(1, b.overlap_set.count(&map));
  EXPECT_EQ(2, b.touch_set.size());
  EXPECT_EQ(1, b.touch_set.count(&c));
  EXPECT_EQ(1, b.touch_set.count(&d));

  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(1, c.touch_set.size());
  EXPECT_EQ(1, c.touch_set.count(&b));

  RunCallbacksOn(tree.get(), &d);
  EXPECT_EQ(1, d.touch_set.size());
  EXPECT_EQ(1, d.touch_set.count(&b));
}

void RectSearchTreeTest::TestFindOutsideBounds() {
  auto tree = RectSearchTree<2, SomeObject<>*>::Create({0, 0, 10, 10}, 1);
  SomeObject<> a({-1, -1, 2, 2}, "a");

  ASSERT_ITER(tree->InsertTrimmed(a.rect, &a));

  SomeObject<> b({-2, -1, 1, 1}, "b");
  ASSERT_ITER(tree->InsertTrimmed(b.rect, &b));

  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(1, b.touch_set.size());
  EXPECT_EQ(1, b.touch_set.count(&a));

  RunCallbacksOn(tree.get(), &a);
  EXPECT_EQ(1, a.touch_set.size());
  EXPECT_EQ(1, a.touch_set.count(&b));
}

void RectSearchTreeTest::Test4D() {
  auto tree = RectSearchTree<4, SomeObject<4>*>::Create(
      {0, 0, 0, 0, 100, 100, 100, 100}, 8);

  SomeObject<4> a({48, 48, 48, 48, 2, 2, 2, 2}, "a");
  tree->Insert(a.rect, &a);

  SomeObject<4> b({50, 50, 50, 50, 2, 2, 2, 2}, "b");
  tree->Insert(b.rect, &b);

  RunCallbacksOn(tree.get(), &a);
  RunCallbacksOn(tree.get(), &b);
  EXPECT_EQ(1, a.touch_set.size());
  EXPECT_EQ(1, a.touch_set.count(&b));
  EXPECT_EQ(1, b.touch_set.size());
  EXPECT_EQ(1, b.touch_set.count(&a));

  SomeObject<4> c({52, 52, 52, 52, 2, 2, 2, 2}, "c");
  tree->Insert(c.rect, &c);
  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(1, c.touch_set.size());
  EXPECT_EQ(1, c.touch_set.count(&b));

  SomeObject<4> d({52, 52, 52, 53, 100, 100, 100, 100}, "d");
  tree->InsertTrimmed(d.rect, &d);
  RunCallbacksOn(tree.get(), &d);
  EXPECT_EQ(2, d.overlap_set.size());
  EXPECT_EQ(1, d.overlap_set.count(&d));
  EXPECT_EQ(1, d.overlap_set.count(&c));
  RunCallbacksOn(tree.get(), &c);
  EXPECT_EQ(2, c.overlap_set.size());
  EXPECT_EQ(1, c.overlap_set.count(&c));
  EXPECT_EQ(1, c.overlap_set.count(&d));
}

void RectSearchTreeTest::TestAllIterator() {
  auto tree = RectSearchTree<2, int>::Create({0, 0, 100, 100}, 2);

  tree->Insert({1, 1, 90, 90}, 4);
  tree->Insert({0, 0, 10, 10}, 5);
  tree->Insert({90, 90, 2, 2}, 6);
  tree->Insert({2, 2, 90, 90}, 7);

  std::unordered_set<int> found;
  // TODO clean up when not debugging
  auto* tree_raw = tree.get();
  for (auto iter = tree_raw->begin(); iter != tree_raw->end(); ++iter)
    found.insert(*iter);

  EXPECT_EQ(4, found.size());
  EXPECT_EQ(1, found.count(4));
  EXPECT_EQ(1, found.count(5));
  EXPECT_EQ(1, found.count(6));
  EXPECT_EQ(1, found.count(7));
}

void RectSearchTreeTest::TestNearIterator() {
  auto tree = RectSearchTree<2, int>::Create({0, 0, 100, 100}, 2);
  tree->Insert({1, 1, 90, 90}, 0);
  tree->Insert({0, 0, 10, 10}, 1);
  tree->Insert({90, 90, 2, 2}, 2);

  std::unordered_set<int> found;
  for (int i : tree->Near({60, 60, 10, 10}))
    found.insert(i);

  EXPECT_EQ(2, found.size());
  EXPECT_EQ(1, found.count(0));
  EXPECT_EQ(0, found.count(1));
  EXPECT_EQ(1, found.count(2));

  found.clear();
  for (int i : tree->Near({40, 40, 20, 20}))
    found.insert(i);

  EXPECT_EQ(3, found.size());
  EXPECT_EQ(1, found.count(0));
  EXPECT_EQ(1, found.count(1));
  EXPECT_EQ(1, found.count(2));
}

void RectSearchTreeTest::TestMove() {
  auto tree = RectSearchTree<2, int>::Create({0, 0, 100, 100}, 2);
  auto iter = tree->Insert({0, 0, 10, 10}, 1);

  std::unordered_set<int> found;
  for (int i : tree->Near({70, 70, 10, 10})) {
    found.insert(i);
    std::cerr << i << std::endl;
  }

  EXPECT_EQ(0, found.size());

  tree->Move(std::move(iter), {72, 72, 10, 10});
  for (int i : tree->Near({70, 70, 10, 10}))
    found.insert(i);

  EXPECT_EQ(1, found.size());
  EXPECT_EQ(1, found.count(1));
}

RectSearchTreeTest::RectSearchTreeTest()
    : TestGroup("RectSearchTreeTest",
                {
                    std::bind(&RectSearchTreeTest::TestCreate, this),
                    std::bind(&RectSearchTreeTest::TestSingleNode, this),
                    std::bind(&RectSearchTreeTest::TestHeight2, this),
                    std::bind(&RectSearchTreeTest::TestFindOutsideBounds, this),
                    std::bind(&RectSearchTreeTest::TestMove, this),
                    std::bind(&RectSearchTreeTest::Test4D, this),
                    std::bind(&RectSearchTreeTest::TestAllIterator, this),
                    std::bind(&RectSearchTreeTest::TestNearIterator, this),
                }) {}

}  // namespace test
}  // namespace engine2