#ifndef ENGINE2_ARENA_TEST_H_
#define ENGINE2_ARENA_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class ArenaTest : public TestGroup {
 public:
  void TestReact();
  void TestMoveActive();
  ArenaTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_ARENA_TEST_H_