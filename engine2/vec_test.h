#ifndef ENGINE2_VEC_TEST_H_
#define ENGINE2_VEC_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class VecTest : public TestGroup {
 public:
  void TestCompile();
  void TestDefaultZeroInit();
  void TestCompare();
  void TestAdd();
  void TestSubtract();
  // TODO test the rest... or is this good enough?

  VecTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_VEC_TEST_H_