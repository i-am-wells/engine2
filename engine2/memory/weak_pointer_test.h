#ifndef ENGINE2_MEMORY_WEAK_POINTER_TEST_H_
#define ENGINE2_MEMORY_WEAK_POINTER_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class WeakPointerTest : public TestGroup {
 public:
  void TestGet();
  void TestDestroyFactory();
  void TestCopy();
  void TestDestroyWeak();

  WeakPointerTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_MEMORY_WEAK_POINTER_TEST_H_