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
  PhysicsObject obj(Rect<>{1, 2, 3, 4}, 56);

  EXPECT_EQ(1, obj.rect.x());
  EXPECT_EQ(2, obj.rect.y());
  EXPECT_EQ(3, obj.rect.w());
  EXPECT_EQ(4, obj.rect.h());

  EXPECT_EQ(56, obj.mass_kg);

  EXPECT_EQ(0, obj.velocity.x());
  EXPECT_EQ(0, obj.velocity.y());

  EXPECT_EQ(0, obj.forces_sum.x());
  EXPECT_EQ(0, obj.forces_sum.y());
}

void PhysicsObjectTest::TestApplyForces() {
  PhysicsObject obj(Rect<>{1, 2, 3, 4}, 56);

  obj.ApplyForce({1, 2});
  EXPECT_EQ(1, obj.forces_sum.x());
  EXPECT_EQ(2, obj.forces_sum.y());

  obj.ApplyForce({3, 4});
  EXPECT_EQ(1 + 3, obj.forces_sum.x());
  EXPECT_EQ(2 + 4, obj.forces_sum.y());
}

void PhysicsObjectTest::TestUpdateZero() {
  PhysicsObject obj(Rect<>{100, 100, 10, 10}, 10);

  // Without forces, nothing should change.
  obj.Update();
  EXPECT_EQ(100, obj.rect.x());
  EXPECT_EQ(100, obj.rect.y());
  EXPECT_EQ(10, obj.rect.w());
  EXPECT_EQ(10, obj.rect.h());

  EXPECT_EQ(10, obj.mass_kg);

  EXPECT_EQ(0, obj.velocity.x());
  EXPECT_EQ(0, obj.velocity.y());

  EXPECT_EQ(0, obj.forces_sum.x());
  EXPECT_EQ(0, obj.forces_sum.y());
}

void PhysicsObjectTest::TestUpdatePositionFromVelocity() {
  PhysicsObject obj(Rect<>{100, 100, 10, 10}, 10);

  obj.velocity = {10, 20};
  Wait(0.1);
  obj.Update();
  EXPECT_EQ(101, obj.rect.x());
  EXPECT_EQ(102, obj.rect.y());
}

void PhysicsObjectTest::TestUpdateVelocityFromForces() {
  PhysicsObject obj(Rect<>{100, 100, 10, 10}, 10);

  obj.ApplyForce({100, 200});
  Wait(0.1);
  obj.Update();
  EXPECT_EQ(1, obj.velocity.x());
  EXPECT_EQ(2, obj.velocity.y());
}

void PhysicsObjectTest::TestGetCollisionTime1D() {
  PhysicsObject a(Rect<>{0, 0, 10, 10}, 1);
  // a moves in the +x direction at 10 pixels/second.
  a.velocity = {10, 0};
  // TODO maybe time should just be 0 by default?
  a.time_seconds = 0;

  // a's leading edge is at x=10, so distance=40. a should touch b after 4
  // seconds.
  PhysicsObject b(Rect<>{50, 0, 10, 10}, 1);
  b.time_seconds = 0;
  EXPECT_EQ(4., GetCollisionTime1D(a, b, 0));
  // a and b don't collide via movement in the y direction.
  EXPECT_EQ(-1., GetCollisionTime1D(a, b, 1));

  // a shouldn't touch c at all.
  PhysicsObject c(Rect<>{50, 30, 10, 10}, 1);
  c.time_seconds = 0;
  EXPECT_EQ(-1., GetCollisionTime1D(a, c, 0));
}

PhysicsObjectTest::PhysicsObjectTest()
    : TestGroup(
          "PhysicsObjectTest",
          {
              std::bind(&PhysicsObjectTest::TestDefault, this),
              /* TODO: disabled until design is final
              std::bind(&PhysicsObjectTest::TestApplyForces, this),
              std::bind(&PhysicsObjectTest::TestUpdateZero, this),
              std::bind(&PhysicsObjectTest::TestUpdatePositionFromVelocity,
                        this),
              std::bind(&PhysicsObjectTest::TestUpdateVelocityFromForces, this),
              */
              std::bind(&PhysicsObjectTest::TestGetCollisionTime1D, this),
          }) {}

}  // namespace test
}  // namespace engine2