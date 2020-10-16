#ifndef ENGINE2_IMPL_LOGIC_CONTEXT_IMPL_H_
#define ENGINE2_IMPL_LOGIC_CONTEXT_IMPL_H_

#include <queue>
#include <vector>

#include <SDL2/SDL_events.h>

#include "engine2/callback_queue.h"
#include "engine2/impl/event_handlers_impl.h"
#include "engine2/logic_context.h"
#include "engine2/memory/weak_pointer.h"
#include "engine2/timing.h"

namespace engine2 {

class LogicContextImpl : public LogicContext {
 public:
  LogicContextImpl();
  ~LogicContextImpl() override;

  void Run(StateMutex* state_mutex) override;
  void Stop() override;
  std::unique_ptr<RunClause> EveryFrame() override;

  std::unique_ptr<RunClause> Every(double num, Timing::TimeUnit unit) override;
  std::unique_ptr<RunClause> After(double num, Timing::TimeUnit unit) override;

  // Keyboard events
  void EnableKeyRepeat(bool enable) override {
    event_handlers_.enable_key_repeat_ = enable;
  }
  std::unique_ptr<KeyboardEventClause> OnKey(SDL_Keycode key_code) override;
  std::unique_ptr<KeyboardEventClause> OnKey(const std::string& name) override;

  // TODO every frame callbacks methods
  std::vector<Callback> every_frame_callbacks_;
  uint32_t ScheduleCallback(Callback callback,
                            double num,
                            Timing::TimeUnit unit,
                            bool repeat);
  void CancelScheduledCallback(uint32_t id);
  void SetKeyDownHandler(SDL_Keycode key_code, KeyboardCallback callback);
  void SetKeyUpHandler(SDL_Keycode key_code, KeyboardCallback callback);

 private:
  void RunEveryFrameCallbacks();
  void HandleSDLEvents();

  WeakPointer<LogicContextImpl> GetWeakPointer();

  CallbackQueue callback_queue_;
  EventHandlersImpl event_handlers_;
  // TODO make configurable
  Timing::FramerateRegulator framerate_regulator_{60};
  bool running_ = false;
  WeakPointer<LogicContextImpl>::Factory weak_factory_{this};
};

}  // namespace engine2

#endif  // ENGINE2_IMPL_LOGIC_CONTEXT_IMPL_H_