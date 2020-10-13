#ifndef ENGINE2_PHYSICS_OBJECT_H_
#define ENGINE2_PHYSICS_OBJECT_H_

#include "engine2/rect.h"

namespace engine2 {

struct PhysicsObject {
 public:
  PhysicsObject(const Rect<>& rect, double mass_kg);

  // Requirements:
  //
  // - Should be able to calculate:
  //    acceleration from sum of forces
  //    velocity from acceleration and current velocity
  //    position from velocity and current position
  //
  // - Should be able to override position, velocity, acceleration for one
  // update cycle. Calculations proceed as usual for the _next_ cycle.

  // for RectSearchTree?
  Rect<> GetRect() const;

  void Update();
  void ApplyForce(const Point<double>& force_vector);

  // Update own forces only;
  void CollideWith(const PhysicsObject& other);

  Rect<double> rect;
  double mass_kg;

  Point<double> velocity = {0, 0};
  // TODO support constant acceleration?
  // TODO support constant force?
  // Point<double> acceleration = {0, 0};
  Point<double> forces_sum = {0, 0};

 private:
  int last_update_ms_;
};

}  // namespace engine2

#endif  // ENGINE2_PHYSICS_OBJECT_H_