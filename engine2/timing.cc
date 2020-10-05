#include "engine2/timing.h"

#include <SDL2/SDL_timer.h>
#include <chrono>
#include <thread>

namespace engine2 {

// static
uint32_t Timing::GetTicks() {
  return SDL_GetTicks();
}

// static
void Timing::Delay(uint32_t milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

Timing::FramerateRegulator::FramerateRegulator(int target_fps) {
  target_frame_ms_ = 1000 / target_fps;
}

void Timing::FramerateRegulator::Wait() {
  uint32_t elapsed = GetTicks() - last_tick_;
  if (elapsed < target_frame_ms_)
    Delay(target_frame_ms_ - elapsed);
  last_tick_ = GetTicks();
}

}  // namespace engine2