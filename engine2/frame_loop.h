#ifndef ENGINE2_FRAME_LOOP_H_
#define ENGINE2_FRAME_LOOP_H_

#include "engine2/callback_queue.h"
#include "engine2/event_handler.h"
#include "engine2/types.h"

namespace engine2 {

class FrameLoop {
 public:
  FrameLoop(EventHandler* event_handler = nullptr,
            CallbackQueue* scheduled_callbacks = nullptr);

  void Run();
  void Stop();

  virtual void EveryFrame() {}

 private:
  bool HandleSDLEvents();
  bool RunEveryFrameHandler();
  bool RunScheduledEvents();

  bool running_ = false;
  EventHandler* event_handler_;
  CallbackQueue* scheduled_callbacks_;
};

}  // namespace engine2

#endif  // ENGINE2_FRAME_LOOP_H_