#include "engine2/space.h"
#include "engine2/physics_object.h"
#include "engine2/rect_object.h"
#include "engine2/space_test.h"
#include "engine2/test/assert_macros.h"

namespace engine2 {
namespace test {
namespace {

constexpr Rect<int64_t, 2> kSpaceRect{0, 0, 1000, 1000};

class ObjectInSpace : public RectObject<2> {
 public:
  ObjectInSpace(double x, double y, double w, double h, double mass)
      : RectObject({x, y, w, h}, mass) {}
  ObjectInSpace(double x,
                double y,
                double w,
                double h,
                double mass,
                std::string name)
      : RectObject({x, y, w, h}, mass), name(name) {}
  void SetVelocity(double vx, double vy) { phys().velocity = {vx, vy}; }

  void OnCollideWith(const ObjectInSpace& other,
                     const Vec<double, 2>& other_velocity,
                     int dimension) {
    ++collide_count;
    physics_.HalfElasticCollision1D(other.physics_, other_velocity, dimension);
  }

  PhysicsObject<2>& phys() { return physics_; }

  int collide_count = 0;
  std::string name = "";
};

class Bar;
class Foo : public ObjectInSpace {
 public:
  int foo_count = 0;
  int bar_count = 0;

  Foo(int x, int y, int w, int h, double mass)
      : ObjectInSpace(x, y, w, h, mass) {}
  void OnCollideWith(const Foo& other,
                     const Vec<double, 2>& other_velocity,
                     int dimension);
  void OnCollideWith(const Bar& other,
                     const Vec<double, 2>& other_velocity,
                     int dimension);
};

class Bar : public ObjectInSpace {
 public:
  int foo_count = 0;
  int bar_count = 0;

  Bar(int x, int y, int w, int h, double mass)
      : ObjectInSpace(x, y, w, h, mass) {}
  void OnCollideWith(const Foo& other,
                     const Vec<double, 2>& other_velocity,
                     int dimension) {
    ++foo_count;
    ObjectInSpace::OnCollideWith(static_cast<const ObjectInSpace&>(other),
                                 other_velocity, dimension);
  }
  void OnCollideWith(const Bar& other,
                     const Vec<double, 2>& other_velocity,
                     int dimension) {
    ++bar_count;
    ObjectInSpace::OnCollideWith(static_cast<const ObjectInSpace&>(other),
                                 other_velocity, dimension);
  }
};

void Foo::OnCollideWith(const Foo& other,
                        const Vec<double, 2>& other_velocity,
                        int dimension) {
  ++foo_count;
  ObjectInSpace::OnCollideWith(static_cast<const ObjectInSpace&>(other),
                               other_velocity, dimension);
}
void Foo::OnCollideWith(const Bar& other,
                        const Vec<double, 2>& other_velocity,
                        int dimension) {
  ++bar_count;
  ObjectInSpace::OnCollideWith(static_cast<const ObjectInSpace&>(other),
                               other_velocity, dimension);
}

}  // namespace

void SpaceTest::TestAdvanceTimeSingle() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1000, 2000);
  space.Add(&a);

  space.AdvanceTime(Time::Delta::FromSeconds(.01));
  EXPECT_EQ(110, a.GetRect().x());
  EXPECT_EQ(120, a.GetRect().y());

  space.AdvanceTime(Time::Delta::FromSeconds(.01));
  EXPECT_EQ(120, a.GetRect().x());
  EXPECT_EQ(140, a.GetRect().y());
}

void SpaceTest::TestAdvanceTimeMultiple() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1000, 2000);
  space.Add(&a);

  ObjectInSpace b(200, 200, 10, 10, 1);
  b.SetVelocity(1000, 0);
  space.Add(&b);

  space.AdvanceTime(Time::Delta::FromSeconds(.01));

  EXPECT_EQ(110, a.GetRect().x());
  EXPECT_EQ(120, a.GetRect().y());
  EXPECT_EQ(210, b.GetRect().x());
  EXPECT_EQ(200, b.GetRect().y());
}

void SpaceTest::TestRemove() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1000, 2000);
  space.Add(&a);

  ObjectInSpace b(200, 200, 10, 10, 1);
  b.SetVelocity(1000, 1000);
  auto b_iter = space.Add(&b);

  space.Remove(b_iter);

  space.AdvanceTime(Time::Delta::FromSeconds(.01));

  EXPECT_EQ(110, a.GetRect().x());
  EXPECT_EQ(120, a.GetRect().y());

  // b was removed so it doesn't get updated.
  EXPECT_EQ(200, b.GetRect().x());
  EXPECT_EQ(200, b.GetRect().y());
}

void SpaceTest::TestNear() {
  Space<2, ObjectInSpace> space({0, 0, 1000, 1000});
  ObjectInSpace a(100, 100, 10, 10, 1);
  space.Add(&a);
  ObjectInSpace b(900, 900, 10, 10, 1);
  space.Add(&b);

  ObjectInSpace* found = nullptr;
  int count = 0;
  for (auto& variant : space.Near({900, 900, 10, 10})) {
    std::visit(
        [&](ObjectInSpace* object) {
          found = object;
          ++count;
        },
        variant);
  }

  EXPECT_EQ(1, count);
  EXPECT_EQ(&b, found);
}

void SpaceTest::TestSimpleCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);
  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1000, 0);
  space.Add(&a);

  ObjectInSpace b(120, 100, 10, 10, 1);
  b.SetVelocity(0, 0);
  space.Add(&b);

  // +-+    +-+
  // |a| -->|b|
  // +-+    +-+
  // a and b collide at t=10. a should stop moving and b should end with
  // v={1,0}.
  space.AdvanceTime(Time::Delta::FromSeconds(.02));

  EXPECT_EQ(1, a.collide_count);
  EXPECT_EQ(1, b.collide_count);

  EXPECT_EQ(110, a.GetRect().x());
  EXPECT_EQ(100, a.GetRect().y());
  EXPECT_EQ(0., a.GetVelocity().x());
  EXPECT_EQ(0., a.GetVelocity().y());

  EXPECT_EQ(130, b.GetRect().x());
  EXPECT_EQ(100, b.GetRect().y());
  EXPECT_EQ(1000., b.GetVelocity().x());
  EXPECT_EQ(0., b.GetVelocity().y());

  // a and b shouldn't collide again.
  space.AdvanceTime(Time::Delta::FromSeconds(.02));

  EXPECT_EQ(1, a.collide_count);
  EXPECT_EQ(1, b.collide_count);

  EXPECT_EQ(110, a.GetRect().x());
  EXPECT_EQ(100, a.GetRect().y());
  EXPECT_EQ(0., a.GetVelocity().x());
  EXPECT_EQ(0., a.GetVelocity().y());

  EXPECT_EQ(150, b.GetRect().x());
  EXPECT_EQ(100, b.GetRect().y());
  EXPECT_EQ(1000., b.GetVelocity().x());
  EXPECT_EQ(0., b.GetVelocity().y());
}

void SpaceTest::TestChainedCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);
  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1000, 0);
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

  space.AdvanceTime(Time::Delta::FromSeconds(.03));

  EXPECT_EQ(1, a.collide_count);
  EXPECT_EQ(2, b.collide_count);
  EXPECT_EQ(1, c.collide_count);

  EXPECT_EQ(110, a.GetRect().x());
  EXPECT_EQ(100, a.GetRect().y());
  EXPECT_EQ(0., a.GetVelocity().x());
  EXPECT_EQ(0., a.GetVelocity().y());

  EXPECT_EQ(130, b.GetRect().x());
  EXPECT_EQ(100, b.GetRect().y());
  EXPECT_EQ(0., b.GetVelocity().x());
  EXPECT_EQ(0., b.GetVelocity().y());

  EXPECT_EQ(150, c.GetRect().x());
  EXPECT_EQ(100, c.GetRect().y());
  EXPECT_EQ(1000., c.GetVelocity().x());
  EXPECT_EQ(0., c.GetVelocity().y());
}

void SpaceTest::TestSimultaneousCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace center(100, 100, 10, 10, 1);
  center.SetVelocity(0, 0);
  space.Add(&center);

  ObjectInSpace left(80, 100, 10, 10, 1);
  left.SetVelocity(1000, 0);
  space.Add(&left);

  ObjectInSpace right(120, 100, 10, 10, 1);
  right.SetVelocity(-1000, 0);
  space.Add(&right);

  // left and right should both collide with center at t=10 and return to their
  // original positions (with reversed velocity). center should stay put.
  space.AdvanceTime(Time::Delta::FromSeconds(.02));

  EXPECT_EQ(100, center.GetRect().x());
  EXPECT_EQ(0, center.GetVelocity().x());

  EXPECT_EQ(80, left.GetRect().x());
  EXPECT_EQ(-1000, left.GetVelocity().x());

  EXPECT_EQ(120, right.GetRect().x());
  EXPECT_EQ(1000, right.GetVelocity().x());
}

void SpaceTest::TestTrolleyCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace left(80, 100, 10, 10, 1, "left");
  left.SetVelocity(1000, 0);
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
  space.AdvanceTime(Time::Delta::FromSeconds(.02));

  EXPECT_EQ(90, left.GetRect().x());
  EXPECT_EQ(0., left.GetVelocity().x());
  EXPECT_EQ(100, a.GetRect().x());
  EXPECT_EQ(0., a.GetVelocity().x());
  EXPECT_EQ(105, b.GetRect().x());
  EXPECT_EQ(0., b.GetVelocity().x());
  EXPECT_EQ(120, c.GetRect().x());
  EXPECT_EQ(1000., c.GetVelocity().x());
}

void SpaceTest::TestMultipleDispatchCollide() {
  Space<2, Foo, Bar> space(kSpaceRect);

  Foo foo_main(100, 100, 100, 100, 10);
  Bar bar_main(300, 100, 100, 100, 10);
  Foo foo_a(100, 90, 1, 1, 1);
  foo_a.SetVelocity(0, 1000);
  Foo foo_b(300, 90, 1, 1, 1);
  foo_b.SetVelocity(0, 1000);
  Bar bar_a(150, 90, 1, 1, 1);
  bar_a.SetVelocity(0, 1000);
  Bar bar_b(350, 90, 1, 1, 1);
  bar_b.SetVelocity(0, 1000);

  space.Add(&foo_main);
  space.Add(&bar_main);
  space.Add(&foo_a);
  space.Add(&foo_b);
  space.Add(&bar_a);
  space.Add(&bar_b);

  space.AdvanceTime(Time::Delta::FromSeconds(.02));

  EXPECT_EQ(1, foo_main.foo_count);
  EXPECT_EQ(1, foo_main.bar_count);
  EXPECT_EQ(1, foo_a.foo_count);
  EXPECT_EQ(1, bar_a.foo_count);

  EXPECT_EQ(1, bar_main.foo_count);
  EXPECT_EQ(1, bar_main.bar_count);
  EXPECT_EQ(1, foo_b.bar_count);
  EXPECT_EQ(1, bar_b.bar_count);
}

void SpaceTest::TestFarFutureNoCollide() {
  Space<2, ObjectInSpace> space(kSpaceRect);

  ObjectInSpace a(100, 100, 10, 10, 1);
  a.SetVelocity(1000, 0);
  space.Add(&a);

  ObjectInSpace b(500, 100, 10, 10, 1);
  b.SetVelocity(0, 0);
  space.Add(&b);

  space.AdvanceTime(Time::Delta::FromSeconds(.001));
  EXPECT_EQ(0, a.collide_count);
}

SpaceTest::SpaceTest()
    : TestGroup("SpaceTest",
                {
                    std::bind(&SpaceTest::TestAdvanceTimeSingle, this),
                    std::bind(&SpaceTest::TestAdvanceTimeMultiple, this),
                    std::bind(&SpaceTest::TestRemove, this),
                    std::bind(&SpaceTest::TestNear, this),
                    std::bind(&SpaceTest::TestSimpleCollide, this),
                    std::bind(&SpaceTest::TestChainedCollide, this),
                    std::bind(&SpaceTest::TestSimultaneousCollide, this),
                    std::bind(&SpaceTest::TestTrolleyCollide, this),
                    std::bind(&SpaceTest::TestFarFutureNoCollide, this),
                    std::bind(&SpaceTest::TestMultipleDispatchCollide, this),
                }) {}

}  // namespace test
}  // namespace engine2