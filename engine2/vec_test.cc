#include "engine2/vec_test.h"
#include "engine2/test/assert_macros.h"
#include "engine2/vec.h"

namespace engine2 {
namespace test {

void VecTest::TestCompile() {
  Vec<int, 1> a{};
  Vec<int, 2> b{4, 5};
  a[0] = 1;
  Vec<int, 3> c{6, 7, 8};
}

void VecTest::TestDefaultZeroInit() {
  Vec<int, 10> v{};
  for (int i = 0; i < 10; ++i)
    EXPECT_EQ(0, v[i]);
}

void VecTest::TestCompare() {
  Vec<double, 3> a{4, 5, 6};
  Vec<double, 3> b{4, 5, 6};
  Vec<double, 3> c{6, 5, 4};
  EXPECT_TRUE(a == b);
  EXPECT_TRUE(b == a);
  EXPECT_FALSE(a == c);
  EXPECT_FALSE(b == c);
}

void VecTest::TestAdd() {
  Vec<int, 2> a{4, 5}, b{6, 7};
  a += b;
  EXPECT_EQ(4 + 6, a.x());
  EXPECT_EQ(5 + 7, a.y());
  EXPECT_EQ(6, b.x());
  EXPECT_EQ(7, b.y());

  Vec<int, 2> c = a + Vec<int, 2>{10, 11};
  EXPECT_EQ(4 + 6 + 10, c.x());
  EXPECT_EQ(5 + 7 + 11, c.y());

  a += 9;
  EXPECT_EQ(4 + 6 + 9, a.x());
  EXPECT_EQ(5 + 7 + 9, a.y());
}

VecTest::VecTest()
    : TestGroup("VecTest",
                {
                    std::bind(&VecTest::TestCompile, this),
                    std::bind(&VecTest::TestCompare, this),
                    std::bind(&VecTest::TestDefaultZeroInit, this),
                    std::bind(&VecTest::TestAdd, this),
                }) {}

}  // namespace test
}  // namespace engine2