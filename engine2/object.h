#ifndef ENGINE2_OBJECT_H_
#define ENGINE2_OBJECT_H_

#include "engine2/rect.h"
#include "engine2/time.h"
#include "engine2/vec.h"

namespace engine2 {

template <int N>
class Object {
 public:
  // Return a Rect containing the object.
  virtual const Rect<double, N>& GetRect() const = 0;

  // Return a Rect that would contain the object if the object were updated by
  // |delta|.
  virtual Rect<double, N> GetRectAfterTime(const Time::Delta& delta) const = 0;

  // Return the object's velocity in pixels per second.
  virtual const Vec<double, N>& GetVelocity() const = 0;

  // Update object by |delta|.
  virtual void Update(const Time::Delta& delta) = 0;

  // Catch-all collision handler: if two Objects collide and they don't provide
  // custom OnCollideWith() implementations, do nothing.
  virtual void OnCollideWith(const Object& other) {}
};

}  // namespace engine2

#endif  // ENGINE2_OBJECT_H_