#include "engine2/state_mutex.h"

namespace engine2 {

std::unique_lock<std::mutex> StateMutex::Lock() {
  return std::unique_lock<std::mutex>(mutex_);
}

void StateMutex::SendSignal(std::unique_lock<std::mutex> lock, Signal signal) {
  {
    auto moved_lock = std::move(lock);
    signal_ = signal;
  }
  condition_variable_.notify_one();
}

StateMutex::WaitAndLockResult StateMutex::WaitAndLock() {
  auto lock = std::unique_lock<std::mutex>(mutex_);
  signal_ = Signal::kWait;
  condition_variable_.wait(lock, [this] { return signal_ != Signal::kWait; });
  return {std::move(lock), signal_};
}

}  // namespace engine2