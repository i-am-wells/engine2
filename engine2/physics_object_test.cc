#include "engine2/physics_object_test.h"
#include "engine2/physics_object.h"
#include "engine2/test/assert_macros.h"
#include "engine2/timing.h"

namespace engine2 {
namespace test {
namespace {

void Wait(double seconds) {
  Timing::Delay(seconds * 1000);
}

}  // namespace

void PhysicsObjectTest::TestDefault() {
  PhysicsObject obj({1, 2, 3, 4}, 56);

  EXPECT_EQ(1, obj.rect.x);
  EXPECT_EQ(2, obj.rect.y);
  EXPECT_EQ(3, obj.rect.w);
  EXPECT_EQ(4, obj.rect.h);

  EXPECT_EQ(56, obj.mass_kg);

  EXPECT_EQ(0, obj.velocity.x);
  EXPECT_EQ(0, obj.velocity.y);

  EXPECT_EQ(0, obj.forces_sum.x);
  EXPECT_EQ(0, obj.forces_sum.y);
}

void PhysicsObjectTest::TestApplyForces() {
  PhysicsObject obj({1, 2, 3, 4}, 56);

  obj.ApplyForce({1, 2});
  EXPECT_EQ(1, obj.forces_sum.x);
  EXPECT_EQ(2, obj.forces_sum.y);

  obj.ApplyForce({3, 4});
  EXPECT_EQ(1 + 3, obj.forces_sum.x);
  EXPECT_EQ(2 + 4, obj.forces_sum.y);
}

void PhysicsObjectTest::TestUpdateZero() {
  PhysicsObject obj({100, 100, 10, 10}, 10);

  // Without forces, nothing should change.
  obj.Update();
  EXPECT_EQ(100, obj.rect.x);
  EXPECT_EQ(100, obj.rect.y);
  EXPECT_EQ(10, obj.rect.w);
  EXPECT_EQ(10, obj.rect.h);

  EXPECT_EQ(10, obj.mass_kg);

  EXPECT_EQ(0, obj.velocity.x);
  EXPECT_EQ(0, obj.velocity.y);

  EXPECT_EQ(0, obj.forces_sum.x);
  EXPECT_EQ(0, obj.forces_sum.y);
}

void PhysicsObjectTest::TestUpdatePositionFromVelocity() {
  PhysicsObject obj({100, 100, 10, 10}, 10);

  obj.velocity = {10, 20};
  Wait(0.1);
  obj.Update();
  EXPECT_EQ(101, obj.rect.x);
  EXPECT_EQ(102, obj.rect.y);
}

void PhysicsObjectTest::TestUpdateVelocityFromForces() {
  PhysicsObject obj({100, 100, 10, 10}, 10);

  obj.ApplyForce({100, 200});
  Wait(0.1);
  obj.Update();
  EXPECT_EQ(1, obj.velocity.x);
  EXPECT_EQ(2, obj.velocity.y);
}

PhysicsObjectTest::PhysicsObjectTest()
    : TestGroup(
          "PhysicsObjectTest",
          {
              std::bind(&PhysicsObjectTest::TestDefault, this),
              std::bind(&PhysicsObjectTest::TestApplyForces, this),
              std::bind(&PhysicsObjectTest::TestUpdateZero, this),
              std::bind(&PhysicsObjectTest::TestUpdatePositionFromVelocity,
                        this),
              std::bind(&PhysicsObjectTest::TestUpdateVelocityFromForces, this),
          }) {}

}  // namespace test
}  // namespace engine2