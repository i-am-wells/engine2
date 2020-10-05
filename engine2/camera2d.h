#ifndef ENGINE2_CAMERA2D_H_
#define ENGINE2_CAMERA2D_H_

#include <vector>

#include "engine2/graphics2d.h"
#include "engine2/offset_graphics2d.h"
#include "engine2/rect.h"

namespace engine2 {

class Camera2D {
 public:
  // |world_rect| represents the position and size of the camera's view into the
  // world in world coordinates. |screen_rect| represents the position and size
  // of the output on the screen, e.g. for an 800x600 window, passing
  // |screen_rect|={0, 0, 800, 600} would cause the camera to draw to the entire
  // screen and passing |screen_rect|={400, 300, 400, 300} would cause the
  // camera to draw to the bottom right corner of the screen.
  Camera2D(Rect world_rect, Rect screen_rect, Graphics2D* graphics = nullptr);
  void SetGraphics(Graphics2D* graphics);

  class Visible {
   public:
    virtual Rect GetRect() = 0;
    virtual void OnCameraDraw(Camera2D* camera) = 0;

    virtual bool operator<(Visible& other);
  };

  // TODO
  // void Follow(Visible* object);

  Graphics2D* InViewCoords() { return &view_graphics_; }
  Graphics2D* InWorldCoords() { return &world_graphics_; }

  // To be called on the draw thread
  void Draw();

  // Needed for RectSearchTree.
  Rect GetRect() { return world_rect_; }
  void OnOverlap(Visible* object);
  void OnTouch(Visible* object);

 private:
  Rect world_rect_;
  Rect screen_rect_;
  OffsetGraphics2D view_graphics_;
  OffsetGraphics2D world_graphics_;
  std::vector<Visible*> objects_;
};

}  // namespace engine2

#endif  // ENGINE2_CAMERA2D_H_