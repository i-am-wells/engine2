#ifndef ENGINE2_THREADS_THREAD_H_
#define ENGINE2_THREADS_THREAD_H_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace engine2 {

class Thread {
 public:
  enum class Signal {
    kWait,
    kGo,
    kQuit,
  };

  // To be called from parent thread:

  // Creates and starts thread.
  explicit Thread(std::function<void()> entry_point);

  std::unique_lock<std::mutex> Lock();

  // Locks, sets signal_, and unlocks.
  void SendSignal(Signal signal);

  void WakeAndJoin();

 protected:
  struct WaitAndLockResult {
    std::unique_lock<std::mutex> lock;
    Signal signal;
  };
  WaitAndLockResult WaitAndLock();

 private:
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cv_;

  Signal signal_ = Signal::kWait;
};

}  // namespace engine2

#endif  // ENGINE2_THREADS_THREAD_H_