#ifndef ENGINE2_SPACE_TEST_H_
#define ENGINE2_SPACE_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class SpaceTest : public TestGroup {
 public:
  void TestAdvanceTimeSingle();
  void TestAdvanceTimeMultiple();
  // TODO test self-remove
  void TestRemove();

  void TestNear();

  void TestSimpleCollide();
  void TestChainedCollide();
  void TestSimultaneousCollide();
  void TestTrolleyCollide();

  void TestMultipleDispatchCollide();
  SpaceTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_SPACE_TEST_H_