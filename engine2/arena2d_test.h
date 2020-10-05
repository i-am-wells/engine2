#ifndef ENGINE2_ARENA2D_TEST_H_
#define ENGINE2_ARENA2D_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class Arena2DTest : public TestGroup {
 public:
  void TestReact();
  void TestMoveActive();
  Arena2DTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_ARENA2D_TEST_H_