#include "engine2/sprite.h"

namespace engine2 {
namespace {

static constexpr Time::Delta kZero{};

}  // namespace

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

Sprite::Sprite(Texture* texture, std::vector<AnimationFrame> frames)
    : texture_(texture), frames_(std::move(frames)) {
  for (const auto& frame : frames_)
    cycle_duration_ += frame.duration;
}

void Sprite::Draw(Graphics2D* graphics, const Rect<int64_t, 2>& dest) {
  AnimationFrame& frame = CurrentFrame();
  graphics->DrawTexture(
      *texture_, frame.source_rect,
      Rect<int64_t, 2>{dest.pos + frame.dest_offset, dest.size});
}

void Sprite::Draw(Graphics2D* graphics, const Point<int64_t, 2>& dest) {
  AnimationFrame& frame = CurrentFrame();
  graphics->DrawTexture(
      *texture_, frame.source_rect,
      Rect<int64_t, 2>{dest + frame.dest_offset, frame.source_rect.size});
}

void Sprite::Draw(Graphics2D* graphics,
                  const Point<int64_t, 2>& dest,
                  double scale) {
  AnimationFrame& frame = CurrentFrame();
  Vec<double, 2> scaled = frame.source_rect.size.ConvertTo<double>() * scale;
  graphics->DrawTexture(
      *texture_, frame.source_rect,
      Rect<int64_t, 2>{dest + frame.dest_offset, scaled.ConvertTo<int64_t>()});
}

void Sprite::Update(const Time& time) {
  if (time <= last_update_time_ || cycle_duration_ == kZero)
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

void Sprite::SetFrames(std::vector<AnimationFrame> frames) {
  frames_ = std::move(frames);
}

}  // namespace engine2