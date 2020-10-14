#ifndef ENGINE2_STATE_MUTEX_H_
#define ENGINE2_STATE_MUTEX_H_

#include <condition_variable>
#include <mutex>

namespace engine2 {

class StateMutex {
 public:
  enum class Signal {
    kWait,
    kGo,
    kQuit,
  };

  std::unique_lock<std::mutex> Lock();

  void SendSignal(std::unique_lock<std::mutex> lock, Signal signal);

  void WakeAndJoin();

  struct WaitAndLockResult {
    std::unique_lock<std::mutex> lock;
    Signal signal;
  };
  WaitAndLockResult WaitAndLock();

 private:
  std::mutex mutex_;
  std::condition_variable condition_variable_;
  Signal signal_;
};

}  // namespace engine2

#endif  // ENGINE2_STATE_MUTEX_H_