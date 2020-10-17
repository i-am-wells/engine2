#include "engine2/vec_test.h"
#include "engine2/test/assert_macros.h"
#include "engine2/vec.h"

namespace engine2 {
namespace test {

void VecTest::TestCompile() {
  Vec<int, 1> a;
  Vec<int, 2> b{4, 5};
  a[0] = 1;
  Vec3<int> c{6, 7, 8};
}

void VecTest::TestCompare() {
  Vec3<double> a{4, 5};
  Vec3<double> b{4, 5};
  Vec3<double> c{6, 5};
  EXPECT_TRUE(a == b);
  EXPECT_TRUE(b == a);
  EXPECT_FALSE(a == c);
  EXPECT_FALSE(b == c);
}

void VecTest::TestAdd() {
  Vec2<int> a{4, 5}, b{6, 7};
  a += b;
  EXPECT_EQ(4 + 6, a.x);
  EXPECT_EQ(5 + 7, a.y);
  EXPECT_EQ(6, b.x);
  EXPECT_EQ(7, b.y);

  Vec2<int> c = a + Vec2<int>{10, 11};
  EXPECT_EQ(4 + 6 + 10, c.x);
  EXPECT_EQ(5 + 7 + 11, c.y);

  a += 9;
  EXPECT_EQ(4 + 6 + 9, a.x);
  EXPECT_EQ(5 + 7 + 9, a.y);
}

VecTest::VecTest()
    : TestGroup("VecTest",
                {
                    std::bind(&VecTest::TestCompile, this),
                    std::bind(&VecTest::TestCompare, this),
                    std::bind(&VecTest::TestAdd, this),
                }) {}

}  // namespace test
}  // namespace engine2