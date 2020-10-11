#include "engine2/camera2d.h"

#include <algorithm>

namespace engine2 {

bool Camera2D::Visible::operator<(Visible& other) {
  return GetRect().y < other.GetRect().y;
}

Camera2D::Camera2D(Rect<> world_rect, Rect<> screen_rect, Graphics2D* graphics)
    : world_rect_(std::move(world_rect)),
      screen_rect_(std::move(screen_rect)),
      view_graphics_(graphics, &screen_rect_.pos),
      world_graphics_(&view_graphics_, &world_rect_.pos) {}

void Camera2D::SetGraphics(Graphics2D* graphics) {
  view_graphics_.SetUnderlyingGraphics2D(graphics);
}

void Camera2D::Draw() {
  std::sort(objects_.begin(), objects_.end(),
            [](Visible* a, Visible* b) { return *a < *b; });
  for (Visible* object : objects_) {
    object->OnCameraDraw(this);
  }
  objects_.clear();
}

void Camera2D::OnOverlap(Visible* object) {
  objects_.push_back(object);
}

void Camera2D::OnTouch(Visible* object) {}

}  // namespace engine2