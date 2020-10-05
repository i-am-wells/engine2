#ifndef ENGINE2_IMPL_ENGINE2_IMPL_H_
#define ENGINE2_IMPL_ENGINE2_IMPL_H_

#include <vector>

#include <SDL2/SDL_events.h>

#include "engine2/engine2.h"
#include "engine2/impl/event_handlers_impl.h"
#include "engine2/memory/weak_pointer.h"
#include "engine2/timing.h"

namespace engine2 {

class Engine2Impl : public Engine2 {
 public:
  explicit Engine2Impl(
      std::unique_ptr<DrawThread::Delegate> draw_thread_delegate = nullptr);
  ~Engine2Impl();

  void Run() override;
  void Stop() override;
  std::unique_ptr<RunClause> EveryFrame() override;

  // Keyboard events
  void EnableKeyRepeat(bool enable) override {
    event_handlers_.enable_key_repeat_ = enable;
  }
  std::unique_ptr<KeyboardEventClause> OnKey(SDL_Keycode key_code) override;
  std::unique_ptr<KeyboardEventClause> OnKey(const std::string& name) override;

  std::vector<Callback> every_frame_callbacks_;
  void SetKeyDownHandler(SDL_Keycode key_code, KeyboardCallback callback);
  void SetKeyUpHandler(SDL_Keycode key_code, KeyboardCallback callback);

 private:
  DrawThread* draw_thread() { return threads_owner_.draw_thread(); }

  void RunEveryFrameCallbacks();
  void HandleSDLEvents();

  WeakPointer<Engine2Impl> GetWeakPointer();

  EventHandlersImpl event_handlers_;
  // TODO make configurable
  Timing::FramerateRegulator framerate_regulator_{60};
  ThreadsOwner threads_owner_;
  bool running_ = false;
  WeakPointer<Engine2Impl>::Factory weak_factory_{this};
};

}  // namespace engine2

#endif  // ENGINE2_IMPL_ENGINE2_IMPL_H_