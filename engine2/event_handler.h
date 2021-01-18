#ifndef ENGINE2_IMPL_EVENT_HANDLER_H_
#define ENGINE2_IMPL_EVENT_HANDLER_H_

#include <SDL2/SDL_events.h>

namespace engine2 {

class EventHandler {
 public:
  void HandleEvent(const SDL_Event& event);

  // Handlers for SDL events.

  // The default implementation for OnQuit() just calls std::exit().
  virtual void OnQuit(const SDL_QuitEvent& event);

  virtual void OnKeyDown(const SDL_KeyboardEvent& event) {}
  virtual void OnKeyUp(const SDL_KeyboardEvent& event) {}

  virtual void OnMouseMotion(const SDL_MouseMotionEvent& event) {}
  virtual void OnMouseButtonDown(const SDL_MouseButtonEvent& event) {}
  virtual void OnMouseButtonUp(const SDL_MouseButtonEvent& event) {}
  virtual void OnMouseWheel(const SDL_MouseWheelEvent& event) {}

  virtual void OnTextEditing(const SDL_TextEditingEvent& event) {}
  virtual void OnTextInput(const SDL_TextInputEvent& event) {}

  virtual void OnJoyAxisMotion(const SDL_JoyAxisEvent& event) {}
  virtual void OnJoyBallMotion(const SDL_JoyBallEvent& event) {}
  virtual void OnJoyHatMotion(const SDL_JoyHatEvent& event) {}
  virtual void OnJoyButtonDown(const SDL_JoyButtonEvent& event) {}
  virtual void OnJoyButtonUp(const SDL_JoyButtonEvent& event) {}
  virtual void OnJoyDeviceAdded(const SDL_JoyDeviceEvent& event) {}
  virtual void OnJoyDeviceRemoved(const SDL_JoyDeviceEvent& event) {}

  virtual void OnControllerAxisMotion(const SDL_ControllerAxisEvent& event) {}
  virtual void OnControllerButtonDown(const SDL_ControllerButtonEvent& event) {}
  virtual void OnControllerButtonUp(const SDL_ControllerButtonEvent& event) {}
  virtual void OnControllerDeviceAdded(const SDL_ControllerDeviceEvent& event) {
  }
  virtual void OnControllerDeviceRemoved(
      const SDL_ControllerDeviceEvent& event) {}
  virtual void OnControllerDeviceRemapped(
      const SDL_ControllerDeviceEvent& event) {}

  virtual void OnFingerDown(const SDL_TouchFingerEvent& event) {}
  virtual void OnFingerUp(const SDL_TouchFingerEvent& event) {}
  virtual void OnFingerMotion(const SDL_TouchFingerEvent& event) {}

  virtual void OnDollarGesture(const SDL_DollarGestureEvent& event) {}
  virtual void OnDollarRecord(const SDL_DollarGestureEvent& event) {}
  virtual void OnMultiGesture(const SDL_MultiGestureEvent& event) {}

  virtual void OnDropFile(const SDL_DropEvent& event) {}
  virtual void OnDropText(const SDL_DropEvent& event) {}
  virtual void OnDropBegin(const SDL_DropEvent& event) {}
  virtual void OnDropComplete(const SDL_DropEvent& event) {}

  virtual void OnClipboardUpdate() {}

  virtual void OnAudioDeviceAdded(const SDL_AudioDeviceEvent& event) {}
  virtual void OnAudioDeviceRemoved(const SDL_AudioDeviceEvent& event) {}

  virtual void OnRenderTargetsReset() {}
  virtual void OnRenderDeviceReset() {}

  virtual void OnKeymapChanged() {}

  virtual void OnAppTerminating() {}
  virtual void OnAppLowMemory() {}
  virtual void OnAppWillEnterBackground() {}
  virtual void OnAppDidEnterBackground() {}
  virtual void OnAppWillEnterForeground() {}
  virtual void OnAppDidEnterForeground() {}

  virtual void OnUserEvent(const SDL_UserEvent& event) {}
  virtual void OnSysWMEvent(const SDL_SysWMEvent& event) {}

  // Checks window event types and calls more specific window events below. If
  // you override this method, you will probably want to call it yourself.
  virtual void OnWindowEvent(const SDL_WindowEvent& event);

  // Window events
  virtual void OnWindowShown(const SDL_WindowEvent& event) {}
  virtual void OnWindowHidden(const SDL_WindowEvent& event) {}
  virtual void OnWindowExposed(const SDL_WindowEvent& event) {}
  virtual void OnWindowMoved(const SDL_WindowEvent& event) {}
  virtual void OnWindowResized(const SDL_WindowEvent& event) {}
  virtual void OnWindowSizeChanged(const SDL_WindowEvent& event) {}
  virtual void OnWindowMinimized(const SDL_WindowEvent& event) {}
  virtual void OnWindowMaximized(const SDL_WindowEvent& event) {}
  virtual void OnWindowRestored(const SDL_WindowEvent& event) {}
  virtual void OnWindowEnter(const SDL_WindowEvent& event) {}
  virtual void OnWindowLeave(const SDL_WindowEvent& event) {}
  virtual void OnWindowFocusGained(const SDL_WindowEvent& event) {}
  virtual void OnWindowFocusLost(const SDL_WindowEvent& event) {}
  virtual void OnWindowClose(const SDL_WindowEvent& event) {}
  virtual void OnWindowTakeFocus(const SDL_WindowEvent& event) {}
  virtual void OnWindowHitTest(const SDL_WindowEvent& event) {}
};

}  // namespace engine2

#endif  // ENGINE2_IMPL_EVENT_HANDLER_H_