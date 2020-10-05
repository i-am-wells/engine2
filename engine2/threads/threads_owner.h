#ifndef ENGINE2_THREADS_THREADS_OWNER_H_
#define ENGINE2_THREADS_THREADS_OWNER_H_

#include <atomic>

#include "engine2/threads/draw_thread.h"
#include "engine2/threads/io_thread.h"

namespace engine2 {

class ThreadsOwner {
 public:
  explicit ThreadsOwner(
      std::unique_ptr<DrawThread::Delegate> draw_thread_delegate);

  void WakeAndJoinAll();

  DrawThread* draw_thread() { return &draw_thread_; }
  IoThread* io_thread() { return &io_thread_; }

 private:
  DrawThread draw_thread_;
  IoThread io_thread_;
};

}  // namespace engine2

#endif  // ENGINE2_THREADS_THREADS_OWNER_H_