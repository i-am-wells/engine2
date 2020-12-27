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

  /* TODO: move to RectObjectTest
  void TestGetCollisionTime1D();
  void TestGetCollisionTime();

  void TestGetCollisionTimeNow();
  */

  void TestUpdateToTime();
  void TestElasticCollision();
  void TestAngledElasticCollision();

  PhysicsObjectTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_PHYSICS_OBJECT_TEST_H_