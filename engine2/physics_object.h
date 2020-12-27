#ifndef ENGINE2_PHYSICS_OBJECT_H_
#define ENGINE2_PHYSICS_OBJECT_H_

#include "engine2/rect.h"
#include "engine2/time.h"

namespace engine2 {

template <int N>
struct PhysicsObject {
 public:
  PhysicsObject(double mass_kg);

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

  double mass_kg;

  // current thoughts:
  // - target_velocity needs to exist. Most games have the player move at full
  // speed immediately and players will expect it.
  //
  // responsibilities of Space:
  //  - have Rect, contain Objects
  //  - enable operations on some or all objects
  //
  // responsibilities of objects:
  //  - change state with:
  //    - passing time
  //    - interactions with other objects
  //    - Hand of God (direct manipulation by creator)
  //

  Point<double, N> velocity{};
  // TODO support constant acceleration?
  // TODO support constant force?
  // Point<double> acceleration = {0, 0};
  Point<double, N> forces_sum{};

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
PhysicsObject<N>::PhysicsObject(double mass_kg) : mass_kg(mass_kg) {}

template <int N>
void PhysicsObject<N>::Update(const Time::Delta& delta) {
  // TODO implement
}

template <int N>
void PhysicsObject<N>::ApplyForce(const Point<double, N>& force_vector) {
  forces_sum += force_vector;
}

}  // namespace engine2

#endif  // ENGINE2_PHYSICS_OBJECT_H_