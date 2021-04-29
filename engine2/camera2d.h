#ifndef ENGINE2_CAMERA2D_H_
#define ENGINE2_CAMERA2D_H_

#include <vector>

#include "engine2/rect.h"
#include "engine2/rect_object.h"

namespace engine2 {

class Camera2D {
 public:
  // |world_rect| represents the position and size of the camera's view into the
  // world in world coordinates. |screen_rect| represents the position and size
  // of the output on the screen, e.g. for an 800x600 window, passing
  // |screen_rect|={0, 0, 800, 600} would cause the camera to draw to the entire
  // screen and passing |screen_rect|={400, 300, 400, 300} would cause the
  // camera to draw to the bottom right corner of the screen.
  Camera2D(Rect<> world_rect, Rect<> screen_rect);

  class Visible {
   public:
    virtual void Draw() = 0;
    virtual bool operator<(Visible& other);
  };

  void SetWorldRect(const Rect<>& rect) { world_rect_ = rect; }
  void SetWindowRect(const Rect<>& rect) { screen_rect_ = rect; }

  const Rect<>& GetWindowRect() const { return screen_rect_; }
  void SetWorldPosition(const Point<int64_t, 2>& position);
  void Move(const Vec<int64_t, 2>& distance);

  // Remain centered on `object`.
  void Follow(RectObject<2>* object);

  // To be called on the draw thread
  void Draw();

  // Needed for RectSearchTree.
  Rect<> GetRect();
  void OnOverlap(Visible* object);
  void OnTouch(Visible* object);

 private:
  RectObject<2>* follow_object_ = nullptr;
  Rect<> world_rect_;
  Rect<> screen_rect_;
  std::vector<Visible*> objects_;
};

}  // namespace engine2

#endif  // ENGINE2_CAMERA2D_H_