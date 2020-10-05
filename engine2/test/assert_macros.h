#ifndef ENGINE2_TEST_ASSERT_MACROS_H_
#define ENGINE2_TEST_ASSERT_MACROS_H_

#include "engine2/base/build_string.h"

namespace engine2 {
namespace test {

#define MSG_PREFIX BuildString(__FILE__, ":", __LINE__, ": ", __func__, ": ")
#define ASSERT(val, msg) Assert(val, MSG_PREFIX + msg)
#define EXPECT(val, msg) Assert(val, MSG_PREFIX + msg, false)

#define ASSERT_EQ(exp, got) \
  ASSERT((exp) == (got), BuildString("Expected ", (exp), " but got ", (got)))
#define ASSERT_NE(a, b) \
  ASSERT((a) != (b), BuildString("Expected ", (a), " != ", (b)))
#define ASSERT_TRUE(val) ASSERT_EQ(true, (val))
#define ASSERT_FALSE(val) ASSERT_EQ(false, (val))
#define ASSERT_NULL(val) \
  ASSERT((val) == nullptr, BuildString("Expected nullptr but got ", (val)))
#define ASSERT_NOT_NULL(val) \
  ASSERT((val) != nullptr, "Expected value to be non-null.")

#define EXPECT_EQ(exp, got) \
  EXPECT((exp) == (got), BuildString("Expected ", (exp), " but got ", (got)))
#define EXPECT_NE(a, b) \
  EXPECT((a) != (b), BuildString("Expected ", (a), " != ", (b)))
#define EXPECT_TRUE(val) EXPECT_EQ(true, (val))
#define EXPECT_FALSE(val) EXPECT_EQ(false, (val))
#define EXPECT_NULL(val) \
  EXPECT((val) == nullptr, BuildString("Expected nullptr but got ", (val)))
#define EXPECT_NOT_NULL(val) \
  EXPECT((val) != nullptr, "Expected value to be non-null.")

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_TEST_ASSERT_MACROS_H_