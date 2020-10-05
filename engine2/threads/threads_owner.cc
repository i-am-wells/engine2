#include "engine2/threads/threads_owner.h"

namespace engine2 {

ThreadsOwner::ThreadsOwner(
    std::unique_ptr<DrawThread::Delegate> draw_thread_delegate)
    : draw_thread_(std::move(draw_thread_delegate)) {}

void ThreadsOwner::WakeAndJoinAll() {
  draw_thread_.WakeAndJoin();
  // TODO
  // io_thread_.WakeAndJoin();
}

}  // namespace engine2