#include "engine2/frame_loop.h"

namespace engine2 {

FrameLoop::FrameLoop(EventHandler* event_handler,
                     CallbackQueue* scheduled_callbacks)
    : event_handler_(event_handler),
      scheduled_callbacks_(scheduled_callbacks) {}

void FrameLoop::Run() {
  running_ = true;
  for (;;) {
    if (!RunEveryFrameHandler())
      break;

    if (!RunScheduledEvents())
      break;

    if (!HandleSDLEvents())
      break;
  }
}

void FrameLoop::Stop() {
  running_ = false;
}

bool FrameLoop::HandleSDLEvents() {
  if (event_handler_) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      event_handler_->HandleEvent(event);
      if (!running_)
        break;
    }
  }
  return running_;
}

bool FrameLoop::RunEveryFrameHandler() {
  EveryFrame();
  return running_;
}

bool FrameLoop::RunScheduledEvents() {
  if (scheduled_callbacks_)
    scheduled_callbacks_->RunCurrent();
  return running_;
}

}  // namespace engine2