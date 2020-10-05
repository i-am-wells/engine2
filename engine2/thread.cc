#include "engine2/threads/thread.h"
#include "engine2/engine2.h"

namespace engine2 {

Thread::Thread(internal::ThreadsOwner* owner, EntryPoint entry_point)
    : owner_(owner), entry_point_(entry_point), thread_(nullptr) {}

void Thread::Start() {
  thread_ = std::make_unique<std::thread>(entry_point_);
}

void Thread::SendSignal(Signal signal) {}

void Thread::WakeAndJoin() {
  if (thread_ && thread_->joinable()) {
    SendSignal(Signal::kQuit);
    thread_->join();
  }
}

bool Thread::Running() {
  return owner_->running() && last_signal_ != Signal::kQuit;
}

}  // namespace engine2