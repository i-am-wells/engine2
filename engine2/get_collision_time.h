#ifndef ENGINE2_COLLISIONS_H_
#define ENGINE2_COLLISIONS_H_

#include "engine2/object.h"
#include "engine2/time.h"

namespace engine2 {

// Note: This calculation only takes velocity into account.
template <int N>
Time GetCollisionTime1D(const Object<N>& a,
                        const Time& t0_a,
                        const Object<N>& b,
                        const Time& t0_b,
                        int d) {
  double vel_a = a.GetVelocity()[d];
  double vel_b = b.GetVelocity()[d];
  if (vel_a == vel_b)
    return Time::FromSeconds(-1);

  // Positions of edges that could collide.
  int64_t pos_a = a.GetRect().pos[d] + a.GetRect().size[d];
  int64_t pos_aa = a.GetRect().pos[d];
  int64_t pos_b = b.GetRect().pos[d];
  int64_t pos_bb = b.GetRect().pos[d] + b.GetRect().size[d];
  // Use the "close" edges
  if (std::abs(pos_aa - pos_bb) < std::abs(pos_a - pos_b)) {
    pos_a = pos_aa;
    pos_b = pos_bb;
  }

  // TODO this is constant velocity only; update for forces/acceleration
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
  // TODO fix Touches() and don't convert to int64
  Rect<int64_t, N> next_rect_a =
      a.GetRectAfterTime(time_final - t0_a).template ConvertTo<int64_t>();
  Rect<int64_t, N> next_rect_b =
      b.GetRectAfterTime(time_final - t0_b).template ConvertTo<int64_t>();

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
CollisionTimeAndDimension GetCollisionTime(const Object<N>& a,
                                           const Time& a_time,
                                           const Object<N>& b,
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

}  // namespace engine2

#endif  // ENGINE2_COLLISIONS_H_