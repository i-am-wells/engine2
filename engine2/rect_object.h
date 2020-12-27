#ifndef ENGINE2_RECT_OBJECT_H_
#define ENGINE2_RECT_OBJECT_H_

#include "engine2/object.h"
#include "engine2/physics_object.h"
#include "engine2/rect.h"

namespace engine2 {

// TODO: just use double for everything and get rid of rect conversions?
template <int N>
class RectObject : public Object<N> {
 public:
  RectObject(Rect<double, N> rect, double mass_kg)
      : rect_(rect), physics_(mass_kg) {}

  const Rect<double, N>& GetRect() const override { return rect_; }
  Rect<double, N> GetRectAfterTime(const Time::Delta& delta) const override;
  const Vec<double, N>& GetVelocity() const override {
    return physics_.velocity;
  }

  void Update(const Time::Delta& delta) override;

 protected:
  Rect<double, N> rect_;
  PhysicsObject<N> physics_;
};

template <int N>
Rect<double, N> RectObject<N>::GetRectAfterTime(
    const Time::Delta& delta) const {
  Rect<double, N> rect_copy = rect_;
  rect_copy.pos += physics_.velocity * delta.ToSeconds();
  return rect_copy;
}

template <int N>
void RectObject<N>::Update(const Time::Delta& delta) {
  rect_ = GetRectAfterTime(delta);
}

}  // namespace engine2

#endif  // ENGINE2_RECT_OBJECT_H_