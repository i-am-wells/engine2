#include "engine2/physics_object.h"
#include "engine2/timing.h"

namespace engine2 {
namespace {

template <typename In, typename Out>
Rect<Out> CastRect(const Rect<In>& rect) {
  return {
      static_cast<Out>(rect.x),
      static_cast<Out>(rect.y),
      static_cast<Out>(rect.w),
      static_cast<Out>(rect.h),
  };
}

}  // namespace

PhysicsObject::PhysicsObject(const Rect<>& rect, double mass_kg)
    : rect(CastRect<int64_t, double>({rect.x, rect.y, rect.w, rect.h})),
      mass_kg(mass_kg),
      last_update_ms_(Timing::GetTicks()) {}

Rect<> PhysicsObject::GetRect() const {
  return CastRect<double, int64_t>(rect);
}

void PhysicsObject::Update() {
  uint32_t time = Timing::GetTicks();
  double elapsed_seconds =
      (time > last_update_ms_) ? (time - last_update_ms_) / 1000. : 1;

  // F = ma
  Point<double> acceleration = forces_sum / mass_kg;
  forces_sum = {0, 0};

  // v = at + v0
  velocity += acceleration * elapsed_seconds;

  // p = vt + p0
  rect.pos += velocity * elapsed_seconds;

  last_update_ms_ = time;
}

void PhysicsObject::ApplyForce(const Point<double>& force_vector) {
  forces_sum += force_vector;
}

}  // namespace engine2