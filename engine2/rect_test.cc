#include <sstream>

#include "engine2/rect.h"
#include "engine2/rect_test.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {

void RectTest::TestConstruct() {
  Rect<> a{1, 2, 3, 4};
  EXPECT_EQ(1, a.x());
  EXPECT_EQ(2, a.y());
  EXPECT_EQ(3, a.w());
  EXPECT_EQ(4, a.h());
}

void RectTest::TestTouches() {
  Rect<> a{10, 10, 5, 5};
  EXPECT_FALSE(a.Touches(a));
  EXPECT_TRUE(a.Touches({9, 10, 1, 1}));
  EXPECT_TRUE(Rect<>({9, 10, 1, 1}).Touches(a));
  EXPECT_FALSE(a.Touches({9, 11, 2, 2}));
  EXPECT_FALSE(a.Touches({11, 11, 2, 2}));
  EXPECT_FALSE(a.Touches({8, 11, 1, 1}));
  EXPECT_TRUE(a.Touches({11, 8, 200, 2}));
  EXPECT_TRUE(a.Touches({15, 15, 5, 5}));
  EXPECT_FALSE(a.Touches({14, 14, 5, 5}));
  EXPECT_FALSE(a.Touches({0, 0, 100, 100}));
}

void RectTest::TestOverlaps() {
  Rect<> a{10, 10, 5, 5};
  EXPECT_TRUE(a.Overlaps(a));
  EXPECT_FALSE(a.Overlaps({9, 10, 1, 1}));
  EXPECT_FALSE(Rect<>({9, 10, 1, 1}).Overlaps(a));
  EXPECT_TRUE(a.Overlaps({9, 11, 2, 2}));
  EXPECT_TRUE(a.Overlaps({11, 11, 2, 2}));
  EXPECT_FALSE(a.Overlaps({8, 11, 1, 1}));
  EXPECT_FALSE(a.Overlaps({11, 8, 200, 2}));
  EXPECT_FALSE(a.Overlaps({15, 15, 5, 5}));
  EXPECT_TRUE(a.Overlaps({14, 14, 5, 5}));
  EXPECT_TRUE(a.Overlaps({0, 0, 100, 100}));
}

void RectTest::TestContainsPoint() {
  Rect<> a{10, 10, 5, 5};
  EXPECT_TRUE(a.Contains(Point<>{10, 10}));
  EXPECT_FALSE(a.Contains(Point<>{9, 10}));
  EXPECT_TRUE(a.Contains(Point<>{12, 12}));
  EXPECT_FALSE(a.Contains(Point<>{100, 100}));
  EXPECT_FALSE(a.Contains(Point<>{12, 15}));
  EXPECT_TRUE(a.Contains(Point<>{12, 14}));
}

void RectTest::TestContainsRect() {
  Rect<> a{10, 10, 5, 5};
  EXPECT_TRUE(a.Contains(a));
  EXPECT_FALSE(a.Contains(Rect<>{9, 10, 1, 1}));
  EXPECT_FALSE(a.Contains(Rect<>{9, 11, 2, 2}));
  EXPECT_TRUE(a.Contains(Rect<>{11, 11, 2, 2}));
  EXPECT_FALSE(a.Contains(Rect<>{8, 11, 1, 1}));
  EXPECT_FALSE(a.Contains(Rect<>{0, 0, 100, 100}));
  EXPECT_TRUE((Rect<>{0, 0, 100, 100}.Contains(a)));
}

void RectTest::TestGetOverlap() {
  Rect<> a{10, 10, 5, 5};
  Rect<> b = a.GetOverlap(a);
  EXPECT_EQ(10, b.x());
  EXPECT_EQ(10, b.y());
  EXPECT_EQ(5, b.w());
  EXPECT_EQ(5, b.h());
  b = {0, 0, 0, 0};

  Rect<> c{0, 0, 100, 100};
  b = a.GetOverlap(c);
  EXPECT_EQ(10, b.x());
  EXPECT_EQ(10, b.y());
  EXPECT_EQ(5, b.w());
  EXPECT_EQ(5, b.h());
  b = {0, 0, 0, 0};

  b = c.GetOverlap(a);
  EXPECT_EQ(10, b.x());
  EXPECT_EQ(10, b.y());
  EXPECT_EQ(5, b.w());
  EXPECT_EQ(5, b.h());
  b = {0, 0, 0, 0};

  Rect<> d{13, 12, 100, 1};
  b = a.GetOverlap(d);
  EXPECT_EQ(13, b.x());
  EXPECT_EQ(12, b.y());
  EXPECT_EQ(2, b.w());
  EXPECT_EQ(1, b.h());
  b = {0, 0, 0, 0};

  Rect<> e{8, 9, 5, 3};
  b = a.GetOverlap(e);
  EXPECT_EQ(10, b.x());
  EXPECT_EQ(10, b.y());
  EXPECT_EQ(3, b.w());
  EXPECT_EQ(2, b.h());
  b = {0, 0, 0, 0};

  Rect<> f{9, 14, 7, 2};
  b = a.GetOverlap(f);
  EXPECT_EQ(10, b.x());
  EXPECT_EQ(14, b.y());
  EXPECT_EQ(5, b.w());
  EXPECT_EQ(1, b.h());
  b = {0, 0, 0, 0};
}

void RectTest::TestGetTouchingDimension() {
  Rect<> a{0, 0, 10, 10};
  Rect<> b{10, 0, 10, 10};
  Rect<> c{10, 10, 5, 10};
  Rect<> d{100, 100, 1, 1};

  EXPECT_EQ(0, a.GetTouchingDimension(b));
  EXPECT_EQ(0, b.GetTouchingDimension(a));

  EXPECT_EQ(0, a.GetTouchingDimension(c));
  EXPECT_EQ(0, c.GetTouchingDimension(a));

  EXPECT_EQ(-1, a.GetTouchingDimension(d));
  EXPECT_EQ(-1, d.GetTouchingDimension(a));

  EXPECT_EQ(1, b.GetTouchingDimension(c));
  EXPECT_EQ(1, c.GetTouchingDimension(b));
}

RectTest::RectTest()
    : TestGroup("RectTest",
                {
                    std::bind(&RectTest::TestConstruct, this),
                    std::bind(&RectTest::TestTouches, this),
                    std::bind(&RectTest::TestOverlaps, this),
                    std::bind(&RectTest::TestContainsPoint, this),
                    std::bind(&RectTest::TestContainsRect, this),
                    std::bind(&RectTest::TestGetOverlap, this),
                    std::bind(&RectTest::TestGetTouchingDimension, this),
                }) {}

}  // namespace test
}  // namespace engine2