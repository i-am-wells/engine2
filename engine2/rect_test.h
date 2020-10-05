#ifndef ENGINE2_RECT_TEST_H_
#define ENGINE2_RECT_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class RectTest : public TestGroup {
 public:
  void TestConstruct();
  void TestTouches();
  void TestOverlaps();
  void TestCorner();
  void TestContainsPoint();
  void TestContainsRect();
  void TestGetOverlap();

  RectTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_RECT_TEST_H_
