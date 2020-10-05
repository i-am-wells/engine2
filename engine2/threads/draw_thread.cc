#include "engine2/threads/draw_thread.h"
#include "engine2/engine2.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

namespace engine2 {
namespace {

std::string DelegateFail(const std::string& method) {
  return "Draw thread delgate: " + method + " returned false.";
}

}  // namespace

DrawThread::DrawThread(std::unique_ptr<DrawThread::Delegate> delegate)
    : Thread(std::bind(&DrawThread::EntryPoint, this)),
      delegate_(std::move(delegate)) {}

bool DrawThread::Okay() const {
  return !early_quit_error_message_;
}

std::string DrawThread::ErrorMessage() const {
  return early_quit_error_message_.value_or(std::string());
}

void DrawThread::EntryPoint() {
  if (SDL_VideoInit(nullptr) < 0) {
    auto lock = std::move(Lock());
    early_quit_error_message_ = SDL_GetError();
    return;
  }

  if (delegate_) {
    if (!delegate_->SetUp()) {
      early_quit_error_message_ = DelegateFail("SetUp()");
      delegate_->TearDown();
      SDL_VideoQuit();
      return;
    }
  }

  while (true) {
    {
      WaitAndLockResult lock = std::move(WaitAndLock());
      if (lock.signal == Signal::kQuit)
        break;

      SDL_PumpEvents();

      if (delegate_) {
        if (!delegate_->ReadState()) {
          early_quit_error_message_ = DelegateFail("ReadState()");
          break;
        }
      }
    }

    if (delegate_) {
      if (!delegate_->Draw()) {
        auto lock = std::move(Lock());
        early_quit_error_message_ = DelegateFail("Draw()");
        break;
      }
    }
  }  // while

  if (delegate_)
    delegate_->TearDown();

  SDL_VideoQuit();
}

/*
void DrawThread::GetSDLEvents(SDL_Event* buffer) {
  SDL_PumpEvents();

  // can be anywhere?
  int read_count = 0;
  for (;;) {
    read_count = SDL_PeepEvents(buffer, 256, SDL_GETEVENT, SDL_FIRSTEVENT,
                                SDL_LASTEVENT);
    if (read_count > 0) {
      HandleEvents(buffer, read_count);
    } else {
      if (read_count < 0) {
        std::cerr << "error in GetEvents: " << SDL_GetError() << std::endl;
      }
      return;
    }
  }
}
*/

}  // namespace engine2