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

void PhysicsObjectTest::TestGetCollisionTime1D() {
  PhysicsObject a(Rect<>{0, 0, 10, 10}, 1);
  // a moves in the +x direction at 10 pixels/second.
  a.velocity = {10, 0};

  Time t0 = Time::FromSeconds(0);

  // a's leading edge is at x=10, so distance=40. a should touch b after 4
  // seconds.
  PhysicsObject b(Rect<>{50, 0, 10, 10}, 1);
  EXPECT_EQ(4., GetCollisionTime1D(a, t0, b, t0, 0).ToSeconds());
  // a and b don't collide via movement in the y direction.
  EXPECT_EQ(-1., GetCollisionTime1D(a, t0, b, t0, 1).ToSeconds());

  // a shouldn't touch c at all.
  PhysicsObject c(Rect<>{50, 30, 10, 10}, 1);
  EXPECT_EQ(-1., GetCollisionTime1D(a, t0, c, t0, 0).ToSeconds());
}

void PhysicsObjectTest::TestGetCollisionTime() {
  PhysicsObject a(Rect<>{0, 0, 10, 10}, 1);
  // a moves in the +x direction at 10 pixels/second.
  a.velocity = {10, 0};

  Time t0 = Time::FromSeconds(0);

  // a's leading edge is at x=10, so distance=40. a should touch b after 4
  // seconds.
  PhysicsObject b(Rect<>{50, 0, 10, 10}, 1);
  auto [collision_time, dimension] = GetCollisionTime(a, t0, b, t0);
  EXPECT_EQ(4., collision_time.ToSeconds());
  EXPECT_EQ(0, dimension);

  // a shouldn't touch c at all.
  PhysicsObject c(Rect<>{50, 30, 10, 10}, 1);
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

void PhysicsObjectTest::TestElasticCollision() {
  // a and b are already touching
  PhysicsObject a(Rect<>{10, 0, 10, 10}, 1);
  a.velocity = {10, 0};
  PhysicsObject b(Rect<>{20, 0, 10, 10}, 1);
  b.velocity = {0, 0};

  PhysicsObject<2>::ElasticCollision(&a, &b);
  EXPECT_EQ(0, a.velocity.x());
  EXPECT_EQ(0, a.velocity.y());

  EXPECT_EQ(10, b.velocity.x());
  EXPECT_EQ(0, b.velocity.y());
}

void PhysicsObjectTest::TestAngledElasticCollision() {
  // a and b are already touching
  PhysicsObject a(Rect<>{10, 0, 10, 10}, 1);
  a.velocity = {1, 1};
  PhysicsObject b(Rect<>{20, 0, 10, 10}, 3);
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
  PhysicsObject a(Rect<>{0, 0, 10, 10}, 1);
  a.velocity = {1, 2};

  a.Update(Time::Delta::FromSeconds(10));
  EXPECT_EQ(10, a.rect.x());
  EXPECT_EQ(20, a.rect.y());
}

void PhysicsObjectTest::TestNoChange() {
  PhysicsObject a(Rect<>{0, 0, 10, 10}, 1);
  a.velocity = {12, 34};
  PhysicsObject b(Rect<>{10, 0, 10, 10}, 1);
  b.velocity = {-56, -78};

  a.CollideWith(b, b.velocity, 0, CollisionOutcome::kNoChange);
  EXPECT_EQ(12, a.velocity.x());
  EXPECT_EQ(34, a.velocity.y());
  EXPECT_EQ(-56, b.velocity.x());
  EXPECT_EQ(-78, b.velocity.y());

  b.CollideWith(a, a.velocity, 0, CollisionOutcome::kNoChange);
  EXPECT_EQ(12, a.velocity.x());
  EXPECT_EQ(34, a.velocity.y());
  EXPECT_EQ(-56, b.velocity.x());
  EXPECT_EQ(-78, b.velocity.y());
}

void PhysicsObjectTest::TestBounceOff() {
  PhysicsObject a(Rect<>{10, 0, 10, 10}, 1);
  a.velocity = {10, 0};
  PhysicsObject b(Rect<>{20, 0, 10, 10}, 1);
  b.velocity = {0, 0};

  Point<double, 2> initial_velocity_a = a.velocity;

  a.CollideWith(b, b.velocity, 0, CollisionOutcome::kBounceOff);
  EXPECT_EQ(0, a.velocity.x());
  EXPECT_EQ(0, a.velocity.y());

  b.CollideWith(a, initial_velocity_a, 0, CollisionOutcome::kBounceOff);
  EXPECT_EQ(10, b.velocity.x());
  EXPECT_EQ(0, b.velocity.y());
}

void PhysicsObjectTest::TestStopDead() {
  PhysicsObject a(Rect<>{10, 0, 10, 10}, 1000);
  a.velocity = {10, 0};
  PhysicsObject b(Rect<>{20, 0, 10, 10}, 1);
  b.velocity = {0, 0};

  Point<double, 2> initial_velocity_a = a.velocity;

  // In bounce mode, a's much greater mass would mean it would keep moving in
  // the same direction after colliding, but in stop-dead-mode it stops.
  a.CollideWith(b, b.velocity, 0, CollisionOutcome::kStopDead);
  EXPECT_EQ(0, a.velocity.x());
  EXPECT_EQ(0, a.velocity.y());

  b.CollideWith(a, initial_velocity_a, 0, CollisionOutcome::kStopDead);
  EXPECT_EQ(0, b.velocity.x());
  EXPECT_EQ(0, b.velocity.y());
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
              std::bind(&PhysicsObjectTest::TestGetCollisionTimeNow, this),
              std::bind(&PhysicsObjectTest::TestGetCollisionTime, this),
              std::bind(&PhysicsObjectTest::TestElasticCollision, this),
              std::bind(&PhysicsObjectTest::TestAngledElasticCollision, this),
              std::bind(&PhysicsObjectTest::TestUpdateToTime, this),
              std::bind(&PhysicsObjectTest::TestNoChange, this),
              std::bind(&PhysicsObjectTest::TestBounceOff, this),
              std::bind(&PhysicsObjectTest::TestStopDead, this),
          }) {}

}  // namespace test
}  // namespace engine2