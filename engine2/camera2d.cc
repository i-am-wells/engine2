#include "engine2/camera2d.h"

#include <algorithm>

namespace engine2 {

// TODO support N dimensions

bool Camera2D::Visible::operator<(Visible& other) {
  return GetRect().y() < other.GetRect().y();
}

Camera2D::Camera2D(Rect<> world_rect, Rect<> screen_rect, Graphics2D* graphics)
    : world_rect_(std::move(world_rect)),
      screen_rect_(std::move(screen_rect)),
      view_graphics_(graphics, &screen_rect_.pos),
      world_graphics_(&view_graphics_, &world_rect_.pos) {}

void Camera2D::SetGraphics(Graphics2D* graphics) {
  view_graphics_.SetUnderlyingGraphics2D(graphics);
}

void Camera2D::SetWorldPosition(const Point<int64_t, 2>& position) {
  world_rect_.pos = position;
}

void Camera2D::Move(const Vec<int64_t, 2>& distance) {
  world_rect_.pos += distance;
}

void Camera2D::Follow(Visible* object) {
  follow_object_ = object;
}

void Camera2D::Draw() {
  std::sort(objects_.begin(), objects_.end(),
            [](Visible* a, Visible* b) { return *a < *b; });
  for (Visible* object : objects_) {
    object->OnCameraDraw(this);
  }
  objects_.clear();
}

Rect<> Camera2D::GetRect() {
  if (follow_object_) {
    Rect follow_rect = follow_object_->GetRect();
    world_rect_.pos =
        follow_rect.pos - (screen_rect_.size - follow_rect.size) / 2l;
  }
  return world_rect_;
}

void Camera2D::OnOverlap(Visible* object) {
  objects_.push_back(object);
}

void Camera2D::OnTouch(Visible* object) {}

}  // namespace engine2