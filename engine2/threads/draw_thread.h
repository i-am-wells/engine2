#ifndef ENGINE2_THREADS_DRAW_THREAD_H_
#define ENGINE2_THREADS_DRAW_THREAD_H_

#include "engine2/threads/thread.h"

namespace engine2 {

class DrawThread : public Thread {
 public:
  class Delegate {
   public:
    // Draw thread will immediately run TearDown() and quit if any of these
    // return false.
    virtual bool SetUp() = 0;
    virtual bool ReadState() = 0;
    virtual bool Draw() = 0;
    virtual void TearDown() = 0;
    virtual ~Delegate() = default;
  };

  explicit DrawThread(std::unique_ptr<Delegate> delegate);

  // Should only be called with mutex_ locked.
  bool Okay() const;
  std::string ErrorMessage() const;

 private:
  void EntryPoint();

  std::optional<std::string> early_quit_error_message_;
  std::unique_ptr<Delegate> delegate_;
};

}  // namespace engine2

#endif  // ENGINE2_THREADS_DRAW_THREAD_H_