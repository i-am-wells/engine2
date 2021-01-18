#ifndef ENGINE2_EVENT_HANDLER_IMPL_H_
#define ENGINE2_EVENT_HANDLER_IMPL_H_

#include <functional>
#include <unordered_map>

#include "engine2/event_handler.h"
#include "engine2/types.h"

namespace engine2 {

class LogicContextImpl;

class EventHandlerImpl : public EventHandler {
 public:
  // TODO add all events here
  void OnQuit(const SDL_QuitEvent& event) override;
  void OnKeyDown(const SDL_KeyboardEvent& event) override;
  void OnKeyUp(const SDL_KeyboardEvent& event) override;

 private:
  friend class LogicContextImpl;

  QuitCallback quit_callback_;

  bool enable_key_repeat_ = true;
  // Map SDL_Keycodes to handlers.
  std::unordered_map<SDL_Keycode, KeyboardCallback> key_down_;
  std::unordered_map<SDL_Keycode, KeyboardCallback> key_up_;
};

}  // namespace engine2

#endif  // ENGINE2_EVENT_HANDLER_IMPL_H_