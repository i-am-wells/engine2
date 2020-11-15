#ifndef ENGINE2_PHYSICS_OBJECT_TEST_H_
#define ENGINE2_PHYSICS_OBJECT_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class PhysicsObjectTest : public TestGroup {
 public:
  void TestDefault();
  void TestApplyForces();
  void TestUpdateZero();
  void TestUpdatePositionFromVelocity();
  void TestUpdateVelocityFromForces();

  void TestGetCollisionTime1D();

  PhysicsObjectTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_PHYSICS_OBJECT_TEST_H_