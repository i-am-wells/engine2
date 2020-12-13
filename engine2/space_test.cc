#include "engine2/space.h"
#include "engine2/physics_object.h"
#include "engine2/space_test.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {
namespace {

constexpr Rect<int64_t, 2> kSpaceRect{0, 0, 1000, 1000};

class ObjectInSpace {
 public:
  ObjectInSpace(int x, int y, int w, int h, double mass)
      : phys({x, y, w, h}, mass) {}
  ObjectInSpace(int x, int y, int w, int h, double mass, std::string name)
      : phys({x, y, w, h}, mass), name(name) {}
  void SetVelocity(double vx, double vy) { phys.velocity = {vx, vy}; }

  void OnCollideWith(const ObjectInSpace& other, double collision_time) {
    ++collide_count;
  }

  // for Space
  PhysicsObject<2>* physics() { return &phys; }

  int collide_count = 0;
  PhysicsObject<2> phys;
  std::string name = "";
};

class Bar;
class Foo : public ObjectInSpace {
 public:
  int foo_count = 0;
  int bar_count = 0;

  Foo(int x, int y, int w, int h, double mass)
      : ObjectInSpace(x, y, w, h, mass) {}
  void OnCollideWith(const Foo& other, double time) { ++foo_count; }
  void OnCollideWith(const Bar& other, double time) { ++bar_count; }
};

class Bar : public ObjectInSpace {
 public:
  int foo_count = 0;
  int bar_count = 0;

  Bar(int x, int y, int w, int h, double mass)
      : ObjectInSpace(x, y, w, h, mass) {}
  void OnCollideWith(const Foo& other, double time) { ++foo_count; }
  void OnCollideWith(const Bar& other, double time) { ++bar_count; }
};

}  // namespace

void SpaceTest::TestAdvanceTimeSingle() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1, 2);
  space.Add(&a);

  space.AdvanceTime(10);
  EXPECT_EQ(110, a.phys.rect.x());
  EXPECT_EQ(120, a.phys.rect.y());

  space.AdvanceTime(20);
  EXPECT_EQ(120, a.phys.rect.x());
  EXPECT_EQ(140, a.phys.rect.y());
}

void SpaceTest::TestAdvanceTimeMultiple() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1, 2);
  space.Add(&a);

  ObjectInSpace b(200, 200, 10, 10, 1);
  b.SetVelocity(1, 0);
  space.Add(&b);

  space.AdvanceTime(10);

  EXPECT_EQ(110, a.phys.rect.x());
  EXPECT_EQ(120, a.phys.rect.y());
  EXPECT_EQ(210, b.phys.rect.x());
  EXPECT_EQ(200, b.phys.rect.y());
}

void SpaceTest::TestRemove() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1, 2);
  space.Add(&a);

  ObjectInSpace b(200, 200, 10, 10, 1);
  b.SetVelocity(1, 1);
  auto b_iter = space.Add(&b);

  space.Remove(b_iter);

  space.AdvanceTime(10);

  EXPECT_EQ(110, a.phys.rect.x());
  EXPECT_EQ(120, a.phys.rect.y());

  // b was removed so it doesn't get updated.
  EXPECT_EQ(200, b.phys.rect.x());
  EXPECT_EQ(200, b.phys.rect.y());
}

void SpaceTest::TestSimpleCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);
  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1, 0);
  space.Add(&a);

  ObjectInSpace b(120, 100, 10, 10, 1);
  b.SetVelocity(0, 0);
  space.Add(&b);

  // +-+    +-+
  // |a| -->|b|
  // +-+    +-+
  // a and b collide at t=10. a should stop moving and b should end with
  // v={1,0}.
  space.AdvanceTime(20);

  EXPECT_EQ(1, a.collide_count);
  EXPECT_EQ(1, b.collide_count);

  EXPECT_EQ(110, a.phys.rect.x());
  EXPECT_EQ(100, a.phys.rect.y());
  EXPECT_EQ(0., a.phys.velocity.x());
  EXPECT_EQ(0., a.phys.velocity.y());

  EXPECT_EQ(130, b.phys.rect.x());
  EXPECT_EQ(100, b.phys.rect.y());
  EXPECT_EQ(1., b.phys.velocity.x());
  EXPECT_EQ(0., b.phys.velocity.y());

  // a and b shouldn't collide again.
  space.AdvanceTime(40);

  EXPECT_EQ(1, a.collide_count);
  EXPECT_EQ(1, b.collide_count);

  EXPECT_EQ(110, a.phys.rect.x());
  EXPECT_EQ(100, a.phys.rect.y());
  EXPECT_EQ(0., a.phys.velocity.x());
  EXPECT_EQ(0., a.phys.velocity.y());

  EXPECT_EQ(150, b.phys.rect.x());
  EXPECT_EQ(100, b.phys.rect.y());
  EXPECT_EQ(1., b.phys.velocity.x());
  EXPECT_EQ(0., b.phys.velocity.y());
}

void SpaceTest::TestChainedCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);
  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1, 0);
  space.Add(&a);
  ObjectInSpace b(120, 100, 10, 10, 1);
  b.SetVelocity(0, 0);
  space.Add(&b);
  ObjectInSpace c(140, 100, 10, 10, 1);
  c.SetVelocity(0, 0);
  space.Add(&c);

  // t=0:  a.x = 100, b.x = 120, c.x = 140
  // t=10: a.x = 110, a hits b   ...
  // t=20: ...        b.x = 130, b hits c
  // t=30: ...        ...        c.x = 150

  space.AdvanceTime(30);

  EXPECT_EQ(1, a.collide_count);
  EXPECT_EQ(2, b.collide_count);
  EXPECT_EQ(1, c.collide_count);

  EXPECT_EQ(110, a.phys.rect.x());
  EXPECT_EQ(100, a.phys.rect.y());
  EXPECT_EQ(0., a.phys.velocity.x());
  EXPECT_EQ(0., a.phys.velocity.y());

  EXPECT_EQ(130, b.phys.rect.x());
  EXPECT_EQ(100, b.phys.rect.y());
  EXPECT_EQ(0., b.phys.velocity.x());
  EXPECT_EQ(0., b.phys.velocity.y());

  EXPECT_EQ(150, c.phys.rect.x());
  EXPECT_EQ(100, c.phys.rect.y());
  EXPECT_EQ(1., c.phys.velocity.x());
  EXPECT_EQ(0., c.phys.velocity.y());
}

void SpaceTest::TestSimultaneousCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace center(100, 100, 10, 10, 1);
  center.SetVelocity(0, 0);
  space.Add(&center);

  ObjectInSpace left(80, 100, 10, 10, 1);
  left.SetVelocity(1, 0);
  space.Add(&left);

  ObjectInSpace right(120, 100, 10, 10, 1);
  right.SetVelocity(-1, 0);
  space.Add(&right);

  // left and right should both collide with center at t=10 and return to their
  // original positions (with reversed velocity). center should stay put.
  space.AdvanceTime(20);

  EXPECT_EQ(100, center.phys.rect.x());
  EXPECT_EQ(0, center.phys.velocity.x());

  EXPECT_EQ(80, left.phys.rect.x());
  EXPECT_EQ(-1, left.phys.velocity.x());

  EXPECT_EQ(120, right.phys.rect.x());
  EXPECT_EQ(1, right.phys.velocity.x());
}

void SpaceTest::TestTrolleyCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace left(80, 100, 10, 10, 1, "left");
  left.SetVelocity(1, 0);
  space.Add(&left);

  ObjectInSpace a(100, 100, 5, 5, 1, "a");
  a.SetVelocity(0, 0);
  space.Add(&a);

  ObjectInSpace b(105, 100, 5, 5, 1, "b");
  b.SetVelocity(0, 0);
  space.Add(&b);

  ObjectInSpace c(110, 100, 5, 5, 1, "c");
  c.SetVelocity(0, 0);
  space.Add(&c);

  // left collides with (a, b, c) and c breaks away.
  space.AdvanceTime(20);

  EXPECT_EQ(90, left.phys.rect.x());
  EXPECT_EQ(0., left.phys.velocity.x());
  EXPECT_EQ(100, a.phys.rect.x());
  EXPECT_EQ(0., a.phys.velocity.x());
  EXPECT_EQ(105, b.phys.rect.x());
  EXPECT_EQ(0., b.phys.velocity.x());
  EXPECT_EQ(120, c.phys.rect.x());
  EXPECT_EQ(1., c.phys.velocity.x());
}

void SpaceTest::TestMultipleDispatchCollide() {
  Space<2, Foo, Bar> space(kSpaceRect);

  Foo foo_main(100, 100, 100, 100, 10);
  Bar bar_main(300, 100, 100, 100, 10);
  Foo foo_a(100, 90, 1, 1, 1);
  Foo foo_b(200, 90, 1, 1, 1);
}

SpaceTest::SpaceTest()
    : TestGroup("SpaceTest",
                {
                    std::bind(&SpaceTest::TestAdvanceTimeSingle, this),
                    std::bind(&SpaceTest::TestAdvanceTimeMultiple, this),
                    std::bind(&SpaceTest::TestRemove, this),
                    std::bind(&SpaceTest::TestSimpleCollide, this),
                    std::bind(&SpaceTest::TestChainedCollide, this),
                    std::bind(&SpaceTest::TestSimultaneousCollide, this),
                    std::bind(&SpaceTest::TestTrolleyCollide, this),
                    std::bind(&SpaceTest::TestMultipleDispatchCollide, this),
                }) {}

}  // namespace test
}  // namespace engine2