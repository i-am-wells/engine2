#ifndef ENGINE2_PHYSICS_OBJECT_H_
#define ENGINE2_PHYSICS_OBJECT_H_

#include "engine2/rect.h"
#include "engine2/timing.h"

namespace engine2 {

template <int N>
struct PhysicsObject {
 public:
  PhysicsObject(const Rect<int64_t, N>& rect, double mass_kg);

  Rect<int64_t, N> GetRect() const;

  void Update();
  void ApplyForce(const Point<double, N>& force_vector);

  // Update own forces only;
  void CollideWith(const PhysicsObject& other);

  Rect<double, N> rect;
  double mass_kg;

  Point<double, N> velocity{};
  // TODO support constant acceleration?
  // TODO support constant force?
  // Point<double> acceleration = {0, 0};
  Point<double, N> forces_sum{};

 private:
  int last_update_ms_;
};

template <int N>
PhysicsObject<N>::PhysicsObject(const Rect<int64_t, N>& rect, double mass_kg)
    : rect(rect.template ConvertTo<double>()),
      mass_kg(mass_kg),
      last_update_ms_(Timing::GetTicks()) {}

template <int N>
Rect<int64_t, N> PhysicsObject<N>::GetRect() const {
  return rect;
}

template <int N>
void PhysicsObject<N>::Update() {
  uint32_t time = Timing::GetTicks();
  double elapsed_seconds =
      (time > last_update_ms_) ? (time - last_update_ms_) / 1000. : 1;

  // F = ma
  Point<double, N> acceleration = forces_sum / mass_kg;
  // Clear forces
  forces_sum = Point<double, N>();

  // v = at + v0
  velocity += acceleration * elapsed_seconds;

  // p = vt + p0
  rect.pos += velocity * elapsed_seconds;

  last_update_ms_ = time;
}

template <int N>
void PhysicsObject<N>::ApplyForce(const Point<double, N>& force_vector) {
  forces_sum += force_vector;
}

}  // namespace engine2

#endif  // ENGINE2_PHYSICS_OBJECT_H_