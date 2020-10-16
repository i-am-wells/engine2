#ifndef ENGINE2_CALLBACK_QUEUE_H_
#define ENGINE2_CALLBACK_QUEUE_H_

#include <cstdint>
#include <vector>

#include "engine2/callback_with_id.h"
#include "engine2/timing.h"

namespace engine2 {

class CallbackQueue {
 public:
  uint32_t Schedule(Callback callback,
                    double num,
                    Timing::TimeUnit unit,
                    bool repeat);
  void Cancel(uint32_t id);
  void RunCurrent();

 private:
  struct ScheduledCallback {
    CallbackWithId callback;
    uint32_t run_time;
    uint32_t repeat_interval;
    bool operator>(const ScheduledCallback& other) const;
  };
  std::vector<ScheduledCallback> scheduled_callbacks_;
  uint32_t next_scheduled_callback_id_ = 0;
};

}  // namespace engine2

#endif  // ENGINE2_CALLBACK_QUEUE_H_