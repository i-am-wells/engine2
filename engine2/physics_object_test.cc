#include "engine2/physics_object_test.h"
#include "engine2/physics_object.h"
#include "engine2/test/assert_macros.h"
#include "engine2/time.h"

namespace engine2 {
namespace test {
namespace {

constexpr Time time(double seconds) {
  return Time::FromSeconds(seconds);
}

}  // namespace

void PhysicsObjectTest::TestDefault() {
  PhysicsObject<2> obj(56);

  EXPECT_EQ(56, obj.mass_kg);

  EXPECT_EQ(0, obj.velocity.x());
  EXPECT_EQ(0, obj.velocity.y());

  EXPECT_EQ(0, obj.forces_sum.x());
  EXPECT_EQ(0, obj.forces_sum.y());
}

void PhysicsObjectTest::TestApplyForces() {
  PhysicsObject<2> obj(56);

  obj.ApplyForce({1, 2});
  EXPECT_EQ(1, obj.forces_sum.x());
  EXPECT_EQ(2, obj.forces_sum.y());

  obj.ApplyForce({3, 4});
  EXPECT_EQ(1 + 3, obj.forces_sum.x());
  EXPECT_EQ(2 + 4, obj.forces_sum.y());
}

/* TODO: move to RectObjectTest
void PhysicsObjectTest::TestGetCollisionTime1D() {
  PhysicsObject<2> a(Rect<>{0, 0, 10, 10}, 1);
  // a moves in the +x direction at 10 pixels/second.
  a.velocity = {10, 0};

  Time t0 = Time::FromSeconds(0);

  // a's leading edge is at x=10, so distance=40. a should touch b after 4
  // seconds.
  PhysicsObject<2> b(Rect<>{50, 0, 10, 10}, 1);
  EXPECT_EQ(4., GetCollisionTime1D(a, t0, b, t0, 0).ToSeconds());
  // a and b don't collide via movement in the y direction.
  EXPECT_EQ(-1., GetCollisionTime1D(a, t0, b, t0, 1).ToSeconds());

  // a shouldn't touch c at all.
  PhysicsObject<2> c(Rect<>{50, 30, 10, 10}, 1);
  EXPECT_EQ(-1., GetCollisionTime1D(a, t0, c, t0, 0).ToSeconds());
}

void PhysicsObjectTest::TestGetCollisionTime() {
  PhysicsObject<2> a(Rect<>{0, 0, 10, 10}, 1);
  // a moves in the +x direction at 10 pixels/second.
  a.velocity = {10, 0};

  Time t0 = Time::FromSeconds(0);

  // a's leading edge is at x=10, so distance=40. a should touch b after 4
  // seconds.
  PhysicsObject<2> b(Rect<>{50, 0, 10, 10}, 1);
  auto [collision_time, dimension] = GetCollisionTime(a, t0, b, t0);
  EXPECT_EQ(4., collision_time.ToSeconds());
  EXPECT_EQ(0, dimension);

  // a shouldn't touch c at all.
  PhysicsObject<2> c(Rect<>{50, 30, 10, 10}, 1);
  auto [collision_time_2, dimension_2] = GetCollisionTime(a, t0, c, t0);
  EXPECT_EQ(-1., collision_time_2.ToSeconds());
}

void PhysicsObjectTest::TestGetCollisionTimeNow() {
  PhysicsObject<2> a(Rect<>{10, 10, 10, 10}, 1);
  a.velocity = {1, 0};
  PhysicsObject<2> b(Rect<>{20, 10, 10, 10}, 1);
  b.velocity = {0, 0};

  Time t0 = Time::FromSeconds(0);

  EXPECT_EQ(0, GetCollisionTime1D(a, t0, b, t0, 0).ToSeconds());
}
*/

void PhysicsObjectTest::TestElasticCollision() {
  // a and b are already touching
  PhysicsObject<2> a(1);
  a.velocity = {10, 0};
  PhysicsObject<2> b(1);
  b.velocity = {0, 0};

  PhysicsObject<2>::ElasticCollision(&a, &b);
  EXPECT_EQ(0, a.velocity.x());
  EXPECT_EQ(0, a.velocity.y());

  EXPECT_EQ(10, b.velocity.x());
  EXPECT_EQ(0, b.velocity.y());
}

void PhysicsObjectTest::TestAngledElasticCollision() {
  // a and b are already touching
  PhysicsObject<2> a(1);
  a.velocity = {1, 1};
  PhysicsObject<2> b(3);
  b.velocity = {0, 0};

  // Test collision affecting all dimensions
  PhysicsObject<2>::ElasticCollision(&a, &b);
  EXPECT_EQ(-0.5, a.velocity.x());
  EXPECT_EQ(-0.5, a.velocity.y());
  EXPECT_EQ(0.5, b.velocity.x());
  EXPECT_EQ(0.5, b.velocity.y());

  a.velocity = {1, 1};
  b.velocity = {0, 0};

  // Test collision affecting one dimension
  PhysicsObject<2>::ElasticCollision1D(&a, &b, 0);
  EXPECT_EQ(-0.5, a.velocity.x());
  EXPECT_EQ(1, a.velocity.y());
  EXPECT_EQ(0.5, b.velocity.x());
  EXPECT_EQ(0, b.velocity.y());
}

void PhysicsObjectTest::TestUpdateToTime() {
  // TODO
}

PhysicsObjectTest::PhysicsObjectTest()
    : TestGroup(
          "PhysicsObjectTest",
          {
              std::bind(&PhysicsObjectTest::TestDefault, this),
              /* TODO: disabled until design is final
              std::bind(&PhysicsObjectTest::TestApplyForces, this),
              std::bind(&PhysicsObjectTest::TestGetCollisionTime1D, this),
              std::bind(&PhysicsObjectTest::TestGetCollisionTimeNow, this),
              std::bind(&PhysicsObjectTest::TestGetCollisionTime, this),
              */
              std::bind(&PhysicsObjectTest::TestElasticCollision, this),
              std::bind(&PhysicsObjectTest::TestAngledElasticCollision, this),
              std::bind(&PhysicsObjectTest::TestUpdateToTime, this),
          }) {}

}  // namespace test
}  // namespace engine2