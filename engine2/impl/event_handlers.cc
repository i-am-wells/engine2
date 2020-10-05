#include "engine2/impl/event_handlers.h"

#include <SDL2/SDL_events.h>

#include <cstdlib>

namespace engine2 {

void EventHandlers::OnQuit(const SDL_QuitEvent& event) {
  std::exit(0);
}

void EventHandlers::OnWindowEvent(const SDL_WindowEvent& event) {
  switch (event.type) {
    case SDL_WINDOWEVENT_SHOWN:
      OnWindowShown(event);
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      OnWindowHidden(event);
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      OnWindowExposed(event);
      break;
    case SDL_WINDOWEVENT_MOVED:
      OnWindowMoved(event);
      break;
    case SDL_WINDOWEVENT_RESIZED:
      OnWindowResized(event);
      break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      OnWindowSizeChanged(event);
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      OnWindowMinimized(event);
      break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      OnWindowMaximized(event);
      break;
    case SDL_WINDOWEVENT_RESTORED:
      OnWindowRestored(event);
      break;
    case SDL_WINDOWEVENT_ENTER:
      OnWindowEnter(event);
      break;
    case SDL_WINDOWEVENT_LEAVE:
      OnWindowLeave(event);
      break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      OnWindowFocusGained(event);
      break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      OnWindowFocusLost(event);
      break;
    case SDL_WINDOWEVENT_CLOSE:
      OnWindowClose(event);
      break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
      OnWindowTakeFocus(event);
      break;
    case SDL_WINDOWEVENT_HIT_TEST:
      OnWindowHitTest(event);
      break;
  }
}

void EventHandlers::HandleEvent(const SDL_Event& event) {
  switch (event.type) {
    case SDL_QUIT:
      OnQuit(event.quit);
      break;
    case SDL_APP_TERMINATING:
      OnAppTerminating();
      break;
    case SDL_APP_LOWMEMORY:
      OnAppLowMemory();
      break;
    case SDL_APP_WILLENTERBACKGROUND:
      OnAppWillEnterBackground();
      break;
    case SDL_APP_DIDENTERBACKGROUND:
      OnAppDidEnterBackground();
      break;
    case SDL_APP_WILLENTERFOREGROUND:
      OnAppWillEnterForeground();
      break;
    case SDL_APP_DIDENTERFOREGROUND:
      OnAppDidEnterForeground();
      break;
    case SDL_WINDOWEVENT:
      OnWindowEvent(event.window);
      break;
    case SDL_SYSWMEVENT:
      OnSysWMEvent(event.syswm);
      break;
    case SDL_KEYDOWN:
      OnKeyDown(event.key);
      break;
    case SDL_KEYUP:
      OnKeyUp(event.key);
      break;
    case SDL_TEXTEDITING:
      OnTextEditing(event.edit);
      break;
    case SDL_TEXTINPUT:
      OnTextInput(event.text);
      break;
    case SDL_KEYMAPCHANGED:
      OnKeymapChanged();
      break;
    case SDL_MOUSEMOTION:
      OnMouseMotion(event.motion);
      break;
    case SDL_MOUSEBUTTONDOWN:
      OnMouseButtonDown(event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      OnMouseButtonUp(event.button);
      break;
    case SDL_MOUSEWHEEL:
      OnMouseWheel(event.wheel);
      break;
    case SDL_JOYAXISMOTION:
      OnJoyAxisMotion(event.jaxis);
      break;
    case SDL_JOYBALLMOTION:
      OnJoyBallMotion(event.jball);
      break;
    case SDL_JOYHATMOTION:
      OnJoyHatMotion(event.jhat);
      break;
    case SDL_JOYBUTTONDOWN:
      OnJoyButtonDown(event.jbutton);
      break;
    case SDL_JOYBUTTONUP:
      OnJoyButtonUp(event.jbutton);
      break;
    case SDL_JOYDEVICEADDED:
      OnJoyDeviceAdded(event.jdevice);
      break;
    case SDL_JOYDEVICEREMOVED:
      OnJoyDeviceRemoved(event.jdevice);
      break;
    case SDL_CONTROLLERAXISMOTION:
      OnControllerAxisMotion(event.caxis);
      break;
    case SDL_CONTROLLERBUTTONDOWN:
      OnControllerButtonDown(event.cbutton);
      break;
    case SDL_CONTROLLERBUTTONUP:
      OnControllerButtonUp(event.cbutton);
      break;
    case SDL_CONTROLLERDEVICEADDED:
      OnControllerDeviceAdded(event.cdevice);
      break;
    case SDL_CONTROLLERDEVICEREMOVED:
      OnControllerDeviceRemoved(event.cdevice);
      break;
    case SDL_CONTROLLERDEVICEREMAPPED:
      OnControllerDeviceRemapped(event.cdevice);
      break;
    case SDL_FINGERDOWN:
      OnFingerDown(event.tfinger);
      break;
    case SDL_FINGERUP:
      OnFingerUp(event.tfinger);
      break;
    case SDL_FINGERMOTION:
      OnFingerMotion(event.tfinger);
      break;
    case SDL_DOLLARGESTURE:
      OnDollarGesture(event.dgesture);
      break;
    case SDL_DOLLARRECORD:
      OnDollarRecord(event.dgesture);
      break;
    case SDL_MULTIGESTURE:
      OnMultiGesture(event.mgesture);
      break;
    case SDL_CLIPBOARDUPDATE:
      OnClipboardUpdate();
      break;
    case SDL_DROPFILE:
      OnDropFile(event.drop);
      break;
    case SDL_DROPTEXT:
      OnDropText(event.drop);
      break;
    case SDL_DROPBEGIN:
      OnDropBegin(event.drop);
      break;
    case SDL_DROPCOMPLETE:
      OnDropComplete(event.drop);
      break;
    case SDL_AUDIODEVICEADDED:
      OnAudioDeviceAdded(event.adevice);
      break;
    case SDL_AUDIODEVICEREMOVED:
      OnAudioDeviceRemoved(event.adevice);
      break;
    case SDL_RENDER_TARGETS_RESET:
      OnRenderTargetsReset();
      break;
    case SDL_RENDER_DEVICE_RESET:
      OnRenderDeviceReset();
      break;
    case SDL_USEREVENT:
      OnUserEvent(event.user);
      break;
  }
}

}  // namespace engine2