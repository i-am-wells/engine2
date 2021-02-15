#include "tools/tilemapeditor/two_finger_touch.h"

using engine2::Point;
using engine2::Vec;

namespace tilemapeditor {
namespace {

double Distance(const Point<double, 2>& a, const Point<double, 2>& b) {
  Vec<double, 2> temp = a - b;
  temp *= temp;
  return sqrt(temp.x() + temp.y());
}

}  // namespace

TwoFingerTouch::TwoFingerTouch(Handler* handler) : handler_(handler) {}

void TwoFingerTouch::OnFingerDown(const SDL_TouchFingerEvent& event) {
  points_.insert({event.fingerId, {event.x, event.y}});
  UpdateCenter();
}

void TwoFingerTouch::OnFingerUp(const SDL_TouchFingerEvent& event) {
  points_.erase(event.fingerId);
  UpdateCenter();
}

void TwoFingerTouch::OnFingerMotion(const SDL_TouchFingerEvent& event) {
  if (points_.size() != 2)
    return;

  Point<double, 2> new_point{event.x, event.y};
  Point<double, 2> prev_point, other_point;

  for (auto iter = points_.begin(); iter != points_.end(); ++iter) {
    if (iter->first == event.fingerId) {
      prev_point = iter->second;
      iter->second = new_point;
    } else {
      other_point = iter->second;
    }
  }

  UpdateCenter();

  handler_->OnDrag(new_point - prev_point);
  handler_->OnPinch(center_, Distance(new_point, other_point) /
                                 Distance(prev_point, other_point));
}

void TwoFingerTouch::UpdateCenter() {
  center_ = {};
  for (auto& [id, point] : points_)
    center_ += point;
  center_ /= points_.size();
}

}  // namespace tilemapeditor