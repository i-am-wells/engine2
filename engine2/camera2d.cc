#include "engine2/camera2d.h"

#include <algorithm>

namespace engine2 {

// TODO support N dimensions

Camera2D::Camera2D(Rect<> world_rect, Rect<> screen_rect)
    : world_rect_(std::move(world_rect)),
      screen_rect_(std::move(screen_rect)) {}

void Camera2D::SetWorldPosition(const Point<int64_t, 2>& position) {
  world_rect_.pos = position;
}

void Camera2D::Move(const Vec<int64_t, 2>& distance) {
  world_rect_.pos += distance;
}

void Camera2D::Follow(RectObject<2>* object) {
  follow_object_ = object;
}

void Camera2D::Draw() {
  std::sort(objects_.begin(), objects_.end(),
            [](Visible* a, Visible* b) { return *a < *b; });
  for (Visible* object : objects_) {
    object->Draw();
  }
  objects_.clear();
}

Rect<> Camera2D::GetRect() {
  if (follow_object_) {
    auto follow_rect = follow_object_->GetRect();
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