#include "engine2/sprite.h"

namespace engine2 {

Sprite::Sprite(Texture* texture,
               const Rect<int64_t, 2>& source_rect,
               Point<int64_t, 2> dest_offset,
               Time::Delta duration)
    : texture_(texture) {
  AddFrame({source_rect, dest_offset, duration});
}

Sprite::Sprite(Texture* texture, int frame_count) : texture_(texture) {
  frames_.reserve(frame_count);
}

void Sprite::Draw(Graphics2D* graphics, const Point<int64_t, 2>& dest) {
  AnimationFrame& frame = CurrentFrame();
  graphics->DrawTexture(
      *texture_, frame.source_rect,
      Rect<int64_t, 2>{dest + frame.dest_offset, frame.source_rect.size});
}

void Sprite::Update(const Time& time) {
  if (time <= last_update_time_)
    return;

  Time::Delta elapsed = (time - last_update_time_) % cycle_duration_;
  elapsed += time_since_frame_start_;

  while (elapsed >= CurrentFrame().duration) {
    elapsed -= CurrentFrame().duration;
    AdvanceFrame();
  }

  time_since_frame_start_ = elapsed;
  last_update_time_ = time;
}

void Sprite::AddFrame(const AnimationFrame& frame) {
  frames_.push_back(frame);
  cycle_duration_ += frame.duration;
}

void Sprite::AdvanceFrame() {
  current_frame_ = (current_frame_ + 1) % frames_.size();
}

}  // namespace engine2