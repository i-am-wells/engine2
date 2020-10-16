#include "engine2/callback_queue.h"

#include <algorithm>

namespace engine2 {

uint32_t CallbackQueue::Schedule(Callback callback,
                                 double num,
                                 Timing::TimeUnit unit,
                                 bool repeat) {
  uint32_t interval = Timing::ToTicks(num, unit);
  scheduled_callbacks_.push_back({
      {callback, ++next_scheduled_callback_id_},
      Timing::GetTicks() + interval,
      repeat ? interval : 0,
  });
  std::push_heap(scheduled_callbacks_.begin(), scheduled_callbacks_.end(),
                 std::greater<>{});
  return next_scheduled_callback_id_;
}

void CallbackQueue::Cancel(uint32_t id) {
  int i = 0;
  for (auto& scheduled : scheduled_callbacks_) {
    if (scheduled.callback.id == id) {
      // Overwrite canceled callback with back() and heapify.
      scheduled_callbacks_[i] = scheduled_callbacks_.back();
      scheduled_callbacks_.pop_back();

      std::make_heap(scheduled_callbacks_.begin() + i,
                     scheduled_callbacks_.end(), std::greater<>{});
      return;
    }
    ++i;
  }
}

void CallbackQueue::RunCurrent() {
  uint32_t now = Timing::GetTicks();
  while (!scheduled_callbacks_.empty() &&
         scheduled_callbacks_[0].run_time <= now) {
    // Run callback
    ScheduledCallback& top = scheduled_callbacks_[0];
    top.callback.callback();

    if (top.repeat_interval) {
      // Reschedule
      top.run_time = now + top.repeat_interval;
      std::make_heap(scheduled_callbacks_.begin(), scheduled_callbacks_.end(),
                     std::greater<>{});
    } else {
      // Remove
      std::pop_heap(scheduled_callbacks_.begin(), scheduled_callbacks_.end(),
                    std::greater<>{});
      scheduled_callbacks_.pop_back();
    }
  }
}

bool CallbackQueue::ScheduledCallback::operator>(
    const ScheduledCallback& other) const {
  return run_time > other.run_time;
}

}  // namespace engine2