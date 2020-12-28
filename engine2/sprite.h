#ifndef ENGINE2_SPRITE_H_
#define ENGINE2_SPRITE_H_

#include "engine2/graphics2d.h"
#include "engine2/point.h"
#include "engine2/rect.h"
#include "engine2/texture.h"
#include "engine2/time.h"

namespace engine2 {

class Sprite {
 public:
  // Create a Sprite with a single frame. Convenient for non-animated sprites.
  Sprite(Texture* texture,
         const Rect<int, 2>& source_rect,
         Point<int, 2> dest_offset = {},
         Time::Delta duration = Time::Delta::FromMicroseconds(0));

  // Create a Sprite with space for frame_count frames reserved.
  Sprite(Texture* texture, int frame_count = 1);

  void Draw(Graphics2D* graphics, const Point<int, 2>& dest);

  void Update(const Time& time);

  struct AnimationFrame {
    Rect<int, 2> source_rect;
    Point<int, 2> dest_offset;
    Time::Delta duration;
  };

  void AddFrame(const AnimationFrame& frame);

  int FrameCount() const { return frames_.size(); }
  AnimationFrame& Frame(int index) { return frames_[index]; }
  AnimationFrame& CurrentFrame() { return frames_[current_frame_]; }

 private:
  void AdvanceFrame();

  Texture* texture_;
  std::vector<AnimationFrame> frames_;
  Time::Delta cycle_duration_;

  int current_frame_ = 0;
  Time::Delta time_since_frame_start_;
  Time last_update_time_;
};

}  // namespace engine2

#endif  // ENGINE2_SPRITE_H_