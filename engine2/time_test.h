#ifndef ENGINE2_TIME_TEST_H_
#define ENGINE2_TIME_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class TimeTest : public TestGroup {
 public:
  TimeTest();
  void TestEquals();
  void TestAdd();
  void TestSubtract();
  void TestMultiply();
  void TestDivide();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_TIME_TEST_H_