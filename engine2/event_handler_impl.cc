#include "engine2/event_handler_impl.h"

namespace engine2 {
namespace {

template <typename E>
void CallIfPresent(const std::function<void(const E&)>& func, const E& event) {
  if (func)
    func(event);
}

void CallIfPresent(const std::unordered_map<SDL_Keycode, KeyboardCallback>& map,
                   const SDL_KeyboardEvent& event) {
  auto iter = map.find(event.keysym.sym);
  if (iter != map.end())
    iter->second(event);
}

}  // namespace

void EventHandlerImpl::OnQuit(const SDL_QuitEvent& event) {
  if (quit_callback_)
    quit_callback_(event);
  else
    EventHandler::OnQuit(event);
}

void EventHandlerImpl::OnKeyDown(const SDL_KeyboardEvent& event) {
  if (event.repeat && !enable_key_repeat_)
    return;
  CallIfPresent(key_down_, event);
}

void EventHandlerImpl::OnKeyUp(const SDL_KeyboardEvent& event) {
  CallIfPresent(key_up_, event);
}

}  // namespace engine2