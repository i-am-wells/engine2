#include "engine2/threads/thread.h"

namespace engine2 {

Thread::Thread(std::function<void()> entry_point) : thread_(entry_point) {}

std::unique_lock<std::mutex> Thread::Lock() {
  return std::unique_lock<std::mutex>(mutex_);
}

void Thread::SendSignal(Signal signal) {
  {
    auto lock = std::unique_lock<std::mutex>(mutex_);
    signal_ = signal;
  }
  cv_.notify_one();
}

void Thread::WakeAndJoin() {
  if (!thread_.joinable())
    return;

  SendSignal(Signal::kQuit);
  thread_.join();
}

Thread::WaitAndLockResult Thread::WaitAndLock() {
  auto lock = std::unique_lock<std::mutex>(mutex_);
  cv_.wait(lock, [this] { return signal_ != Signal::kWait; });
  return {std::move(lock), signal_};
}

}  // namespace engine2