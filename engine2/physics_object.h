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
  Rect<int64_t, N> GetRectAtTime(double other_seconds) const;

  void Update();

  void UpdateToTime(double seconds);

  void ApplyForce(const Point<double, N>& force_vector);

  static void ElasticCollision(PhysicsObject<N>* a, PhysicsObject<N>* b);

  Rect<double, N> rect;
  double mass_kg;

  Point<double, N> velocity{};
  // TODO support constant acceleration?
  // TODO support constant force?
  // Point<double> acceleration = {0, 0};
  Point<double, N> forces_sum{};

  double time_seconds;

 private:
  void HalfElasticCollision(const PhysicsObject& other,
                            const Point<double, N>& other_vel_initial);
};

template <int N>
void PhysicsObject<N>::HalfElasticCollision(
    const PhysicsObject& other,
    const Point<double, N>& other_vel_initial) {
  velocity = velocity * (mass_kg - other.mass_kg) +
             other_vel_initial * 2. * other.mass_kg;
  velocity /= (mass_kg + other.mass_kg);
}

// static
template <int N>
void PhysicsObject<N>::ElasticCollision(PhysicsObject<N>* a,
                                        PhysicsObject<N>* b) {
  Point<double, N> vel_a_initial = a->velocity;
  a->HalfElasticCollision(*b, b->velocity);
  b->HalfElasticCollision(*a, vel_a_initial);
}

template <int N>
double GetCollisionTime1D(const PhysicsObject<N>& a,
                          const PhysicsObject<N>& b,
                          int d) {
  double vel_a = a.velocity[d];
  double vel_b = b.velocity[d];
  if (vel_a == vel_b)
    return -1;

  // Positions of edges that could collide.
  int64_t pos_a = a.rect.pos[d] + a.rect.size[d];
  int64_t pos_b = b.rect.pos[d];

  // initial times
  double t0_a = a.time_seconds;
  double t0_b = b.time_seconds;

  // TODO this is constant velocity only; update for forces
  int64_t pos_final =
      (vel_b * (vel_a * (t0_a - t0_b) - pos_a) + vel_a * pos_b) /
      (vel_a - vel_b);

  double time_final;
  if (!vel_a)
    time_final = ((pos_final - pos_b) / vel_b) + t0_b;
  else
    time_final = ((pos_final - pos_a) / vel_a) + t0_a;

  // Check whether there's actually a collision at time_final
  // TODO store this somewhere?
  Rect<int64_t, N> next_rect_a = a.GetRectAtTime(time_final);
  Rect<int64_t, N> next_rect_b = b.GetRectAtTime(time_final);

  if (!next_rect_a.Touches(next_rect_b))
    return -1;

  return time_final;
}

// Return the collision time if there's a collision or -1 if the objects never
// collide.
template <int N>
double GetCollisionTime(const PhysicsObject<N>& a, const PhysicsObject<N>& b) {
  double double_max = std::numeric_limits<double>::max();
  double collision_time = double_max;
  for (int i = 0; i < N; ++i) {
    double maybe_collision_time = GetCollisionTime1D(a, b, i);
    if (maybe_collision_time >= 0 && maybe_collision_time < collision_time)
      collision_time = maybe_collision_time;

    maybe_collision_time = GetCollisionTime1D(b, a, i);
    if (maybe_collision_time >= 0 && maybe_collision_time < collision_time)
      collision_time = maybe_collision_time;
  }
  if (collision_time == double_max)
    return -1;
  return collision_time;
}

template <int N>
PhysicsObject<N>::PhysicsObject(const Rect<int64_t, N>& rect, double mass_kg)
    : rect(rect.template ConvertTo<double>()),
      mass_kg(mass_kg),
      time_seconds(Timing::GetTicks() / 1000) {}

template <int N>
Rect<int64_t, N> PhysicsObject<N>::GetRect() const {
  return rect;
}

template <int N>
Rect<int64_t, N> PhysicsObject<N>::GetRectAtTime(double other_seconds) const {
  double dt = other_seconds - time_seconds;

  Rect<int64_t, N> result;
  for (int i = 0; i < N; ++i) {
    result.pos[i] = rect.pos[i] + dt * velocity[i];
    // TODO include forces, constant accel. etc

    // for now, size doesn't change with time
    result.size[i] = rect.size[i];
  }
  return result;
}

// TODO separate constant accel. and velocity; just update position here
template <int N>
void PhysicsObject<N>::Update() {
  // TODO this should be a parameter
  double time = Timing::GetTicks() / 1000;
  double elapsed_seconds = (time > time_seconds) ? (time - time_seconds) : 1;

  // F = ma
  Point<double, N> acceleration = forces_sum / mass_kg;
  // Clear forces
  forces_sum = Point<double, N>();

  // v = at + v0
  velocity += acceleration * elapsed_seconds;

  // p = vt + p0
  rect.pos += velocity * elapsed_seconds;

  time_seconds = time;
}

template <int N>
void PhysicsObject<N>::UpdateToTime(double seconds) {
  // TODO what about acceleration?
  double dt = seconds - time_seconds;
  rect.pos += velocity * dt;
  time_seconds = seconds;
}

template <int N>
void PhysicsObject<N>::ApplyForce(const Point<double, N>& force_vector) {
  forces_sum += force_vector;
}

}  // namespace engine2

#endif  // ENGINE2_PHYSICS_OBJECT_H_