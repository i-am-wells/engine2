#include "engine2/time.h"
#include "engine2/test/assert_macros.h"
#include "engine2/time_test.h"

namespace engine2 {
namespace test {

void TimeTest::TestEquals() {
  EXPECT_TRUE(Time::FromSeconds(1) == Time::FromSeconds(1));
  EXPECT_TRUE(Time::FromMicroseconds(4) == Time::FromMicroseconds(4));
  EXPECT_TRUE(Time::Delta::FromSeconds(1) == Time::Delta::FromSeconds(1));
  EXPECT_TRUE(Time::Delta::FromMicroseconds(4) ==
              Time::Delta::FromMicroseconds(4));
}
void TimeTest::TestAdd() {
  Time a = Time::FromSeconds(5);
  Time b = Time::FromSeconds(4);
  Time::Delta da = Time::Delta::FromSeconds(1);
  Time::Delta db = Time::Delta::FromSeconds(-1);

  EXPECT_TRUE(b == a + db);
  EXPECT_TRUE(da + db == Time::Delta::FromSeconds(0));
}
void TimeTest::TestSubtract() {
  Time a = Time::FromSeconds(5);
  Time b = Time::FromSeconds(4);
  Time::Delta da = Time::Delta::FromSeconds(1);
  Time::Delta db = Time::Delta::FromSeconds(-1);

  EXPECT_TRUE(da == a - b);
  EXPECT_TRUE(db == b - a);
  EXPECT_TRUE(b == a - da);
  EXPECT_TRUE(da - db == Time::Delta::FromSeconds(2));
}
void TimeTest::TestMultiply() {
  Time::Delta a = Time::Delta::FromSeconds(1);
  EXPECT_TRUE(a * 5 == Time::Delta::FromSeconds(5));
}
void TimeTest::TestDivide() {
  Time::Delta a = Time::Delta::FromSeconds(1);
  EXPECT_TRUE(a / 5 == Time::Delta::FromSeconds(0.2));
}

TimeTest::TimeTest()
    : TestGroup("TimeTest",
                {
                    std::bind(&TimeTest::TestEquals, this),
                    std::bind(&TimeTest::TestAdd, this),
                    std::bind(&TimeTest::TestSubtract, this),
                    std::bind(&TimeTest::TestMultiply, this),
                    std::bind(&TimeTest::TestDivide, this),
                }) {}

}  // namespace test
}  // namespace engine2