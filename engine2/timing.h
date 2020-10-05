#ifndef ENGINE2_TIMING_H_
#define ENGINE2_TIMING_H_

#include <cstdint>

namespace engine2 {

class Timing {
 public:
  static uint32_t GetTicks();
  static void Delay(uint32_t milliseconds);

  enum class TimeUnit {
    kMicroseconds,
    kMilliseconds,
    kSeconds,
    kMinutes,
    kHours,
    kDays
  };

  class FramerateRegulator {
   public:
    explicit FramerateRegulator(int target_fps);
    void Wait();

   private:
    uint32_t last_tick_ = 0;
    uint32_t target_frame_ms_;
  };
};

}  // namespace engine2

#endif  // ENGINE2_TIMING_H_