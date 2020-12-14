#ifndef ENGINE2_PHYSICS_OBJECT_H_
#define ENGINE2_PHYSICS_OBJECT_H_

#include "engine2/rect.h"
#include "engine2/time.h"

namespace engine2 {

template <int N>
struct PhysicsObject {
 public:
  PhysicsObject(const Rect<int64_t, N>& rect, double mass_kg);

  Rect<int64_t, N> GetRect() const;
  Rect<int64_t, N> GetRectAfterTime(const Time::Delta& delta) const;

  void Update(const Time::Delta& delta);

  void ApplyForce(const Point<double, N>& force_vector);

  void HalfElasticCollision(double other_mass_kg,
                            const Point<double, N>& other_vel_initial);

  void HalfElasticCollision1D(double other_mass_kg,
                              const Point<double, N>& other_vel_initial,
                              int d);

  static void ElasticCollision(PhysicsObject<N>* a, PhysicsObject<N>* b);
  static void ElasticCollision1D(PhysicsObject<N>* a,
                                 PhysicsObject<N>* b,
                                 int d);

  Rect<double, N> rect;
  double mass_kg;

  Point<double, N> velocity{};
  // TODO support constant acceleration?
  // TODO support constant force?
  // Point<double> acceleration = {0, 0};
  Point<double, N> forces_sum{};

 private:
  void HalfElasticCollision(const PhysicsObject& other,
                            const Point<double, N>& other_vel_initial);

  void HalfElasticCollision1D(const PhysicsObject& other,
                              const Point<double, N>& other_vel_initial,
                              int d);
};

// TODO call 1d version
template <int N>
void PhysicsObject<N>::HalfElasticCollision(
    double other_mass_kg,
    const Point<double, N>& other_vel_initial) {
  velocity = velocity * (mass_kg - other_mass_kg) +
             other_vel_initial * 2. * other_mass_kg;
  velocity /= (mass_kg + other_mass_kg);
}

template <int N>
void PhysicsObject<N>::HalfElasticCollision1D(
    double other_mass_kg,
    const Point<double, N>& other_vel_initial,
    int d) {
  velocity[d] = velocity[d] * (mass_kg - other_mass_kg) +
                other_vel_initial[d] * 2. * other_mass_kg;
  velocity[d] /= (mass_kg + other_mass_kg);
}

template <int N>
void PhysicsObject<N>::HalfElasticCollision(
    const PhysicsObject& other,
    const Point<double, N>& other_vel_initial) {
  HalfElasticCollision(other.mass_kg, other_vel_initial);
}

template <int N>
void PhysicsObject<N>::HalfElasticCollision1D(
    const PhysicsObject& other,
    const Point<double, N>& other_vel_initial,
    int d) {
  HalfElasticCollision1D(other.mass_kg, other_vel_initial, d);
}

// static
template <int N>
void PhysicsObject<N>::ElasticCollision(PhysicsObject<N>* a,
                                        PhysicsObject<N>* b) {
  Point<double, N> vel_a_initial = a->velocity;
  a->HalfElasticCollision(*b, b->velocity);
  b->HalfElasticCollision(*a, vel_a_initial);
}

// static
template <int N>
void PhysicsObject<N>::ElasticCollision1D(PhysicsObject<N>* a,
                                          PhysicsObject<N>* b,
                                          int d) {
  Point<double, N> vel_a_initial = a->velocity;
  a->HalfElasticCollision1D(*b, b->velocity, d);
  b->HalfElasticCollision1D(*a, vel_a_initial, d);
}

template <int N>
Time GetCollisionTime1D(const PhysicsObject<N>& a,
                        const Time& t0_a,
                        const PhysicsObject<N>& b,
                        const Time& t0_b,
                        int d) {
  double vel_a = a.velocity[d];
  double vel_b = b.velocity[d];
  if (vel_a == vel_b)
    return Time::FromSeconds(-1);

  // Positions of edges that could collide.
  int64_t pos_a = a.rect.pos[d] + a.rect.size[d];
  int64_t pos_aa = a.rect.pos[d];
  int64_t pos_b = b.rect.pos[d];
  int64_t pos_bb = b.rect.pos[d] + b.rect.size[d];
  // Use the "close" edges
  if (std::abs(pos_aa - pos_bb) < std::abs(pos_a - pos_b)) {
    pos_a = pos_aa;
    pos_b = pos_bb;
  }

  // TODO this is constant velocity only; update for forces
  Time::Delta t0_ab_diff = t0_a - t0_b;
  int64_t pos_final =
      (vel_b * (vel_a * t0_ab_diff.ToSeconds() - pos_a) + vel_a * pos_b) /
      (vel_a - vel_b);

  Time time_final;
  if (!vel_a)
    time_final = Time::Delta::FromSeconds((pos_final - pos_b) / vel_b) + t0_b;
  else
    time_final = Time::Delta::FromSeconds((pos_final - pos_a) / vel_a) + t0_a;

  // Check whether there's actually a collision at time_final
  // TODO store this somewhere?
  Rect<int64_t, N> next_rect_a = a.GetRectAfterTime(time_final - t0_a);
  Rect<int64_t, N> next_rect_b = b.GetRectAfterTime(time_final - t0_b);

  if (!next_rect_a.Touches(next_rect_b))
    return Time::FromSeconds(-1);

  return time_final;
}

struct CollisionTimeAndDimension {
  Time time;
  int dimension;
};

// Return the (absolute) collision time if there's a collision or -1 if the
// objects never collide. Returned value will be greater than both
// a_time_seconds and b_time_seconds.
template <int N>
CollisionTimeAndDimension GetCollisionTime(const PhysicsObject<N>& a,
                                           const Time& a_time,
                                           const PhysicsObject<N>& b,
                                           const Time& b_time) {
  Time time_max = Time::FromMicroseconds(std::numeric_limits<int64_t>::max());
  CollisionTimeAndDimension result{time_max, -1};
  for (int i = 0; i < N; ++i) {
    Time time = GetCollisionTime1D(a, a_time, b, b_time, i);
    if (time >= a_time && time >= b_time && time < result.time) {
      result.time = time;
      result.dimension = i;
    }
  }
  if (result.time == time_max)
    return {Time::FromSeconds(-1), -1};
  return result;
}

template <int N>
PhysicsObject<N>::PhysicsObject(const Rect<int64_t, N>& rect, double mass_kg)
    : rect(rect.template ConvertTo<double>()), mass_kg(mass_kg) {}

template <int N>
Rect<int64_t, N> PhysicsObject<N>::GetRect() const {
  return rect.template ConvertTo<int64_t>();
}

template <int N>
Rect<int64_t, N> PhysicsObject<N>::GetRectAfterTime(
    const Time::Delta& delta) const {
  Rect<int64_t, N> result;
  for (int i = 0; i < N; ++i) {
    result.pos[i] = rect.pos[i] + delta.ToSeconds() * velocity[i];
    // TODO include forces, constant accel. etc

    // for now, size doesn't change with time
    result.size[i] = rect.size[i];
  }
  return result;
}

template <int N>
void PhysicsObject<N>::Update(const Time::Delta& delta) {
  // TODO what about acceleration?
  rect.pos += velocity * delta.ToSeconds();
}

template <int N>
void PhysicsObject<N>::ApplyForce(const Point<double, N>& force_vector) {
  forces_sum += force_vector;
}

}  // namespace engine2

#endif  // ENGINE2_PHYSICS_OBJECT_H_