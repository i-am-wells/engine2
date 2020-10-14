#include "engine2/impl/engine2_impl.h"

#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

namespace engine2 {
namespace {

void StripKeyboardEvent(Callback callback, const SDL_KeyboardEvent& event) {
  callback();
}

class EveryFrameCancelable : public Engine2::Cancelable {
 public:
  EveryFrameCancelable(WeakPointer<Engine2Impl> context, Callback callback)
      : context_(std::move(context)), callback_(callback) {}
  void Cancel() override {
    if (!context_)
      return;

    int i = 0;
    for (auto callback : context_->every_frame_callbacks_) {
      if (&callback == &callback_) {
        context_->every_frame_callbacks_.erase(
            context_->every_frame_callbacks_.begin() + i);
        return;
      }
      ++i;
    }
  }
  ~EveryFrameCancelable() override = default;

 private:
  WeakPointer<Engine2Impl> context_;
  Callback callback_;
};

class EveryFrameRunClause : public Engine2::RunClause {
 public:
  explicit EveryFrameRunClause(WeakPointer<Engine2Impl> context)
      : context_(context) {}
  std::unique_ptr<Engine2::Cancelable> Run(Callback callback) override {
    // TODO: log warning if context_ is null
    if (context_)
      context_->every_frame_callbacks_.push_back(callback);

    return std::make_unique<EveryFrameCancelable>(context_, callback);
  }
  ~EveryFrameRunClause() override = default;

 private:
  WeakPointer<Engine2Impl> context_;
};

class KeyboardEventClauseImpl : public Engine2::KeyboardEventClause {
 public:
  explicit KeyboardEventClauseImpl(WeakPointer<Engine2Impl> context,
                                   SDL_Keycode key_code)
      : context_(std::move(context)), key_code_(key_code) {}

  WeakPointer<KeyboardEventClause> Pressed(KeyboardCallback callback) override {
    if (context_)
      context_->SetKeyDownHandler(key_code_, callback);
    return GetWeakPointer();
  }

  WeakPointer<KeyboardEventClause> Pressed(Callback callback) override {
    return Pressed(Bind(std::move(callback)));
  }

  WeakPointer<KeyboardEventClause> Released(
      KeyboardCallback callback) override {
    if (context_)
      context_->SetKeyUpHandler(key_code_, callback);
    return GetWeakPointer();
  }

  WeakPointer<KeyboardEventClause> Released(Callback callback) override {
    return Released(Bind(std::move(callback)));
  }

  ~KeyboardEventClauseImpl() override = default;

 private:
  KeyboardCallback Bind(Callback callback) {
    return std::bind(&StripKeyboardEvent, std::move(callback),
                     std::placeholders::_1);
  }

  WeakPointer<KeyboardEventClause> GetWeakPointer() {
    WeakPointer<KeyboardEventClause> weak;
    weak_factory_.GetWeakPointer(&weak);
    return weak;
  }

  WeakPointer<Engine2Impl> context_;
  SDL_Keycode key_code_;
  WeakPointer<KeyboardEventClause>::Factory weak_factory_{this};
};

}  // namespace

Engine2Impl::Engine2Impl() {
  // TODO SDL inits here
}

Engine2Impl::~Engine2Impl() {
  // TODO SDL quits here
}

void Engine2Impl::Run(StateMutex* state_mutex) {
  if (running_)
    return;

  running_ = true;
  while (running_) {
    {
      auto draw_lock = std::move(state_mutex->Lock());

      // TODO:
      // update
      // handle events
      // handle delayed task queue
      RunEveryFrameCallbacks();

      HandleSDLEvents();

      // TODO: instead, broadcast on cv
      state_mutex->SendSignal(std::move(draw_lock), StateMutex::Signal::kGo);
    }

    framerate_regulator_.Wait();
  }
  // in case we exited the loop with break
  running_ = false;
}

void Engine2Impl::Stop() {
  running_ = false;
}

void Engine2Impl::RunEveryFrameCallbacks() {
  for (auto& callback : every_frame_callbacks_)
    callback();
}

void Engine2Impl::HandleSDLEvents() {
  static constexpr int kEventBufferSize = 256;
  SDL_Event event_buffer[kEventBufferSize];

  int read_count = 0;
  while (true) {
    read_count = SDL_PeepEvents(event_buffer, kEventBufferSize, SDL_GETEVENT,
                                SDL_FIRSTEVENT, SDL_LASTEVENT);
    if (read_count > 0) {
      for (size_t i = 0; i < read_count; ++i)
        event_handlers_.HandleEvent(event_buffer[i]);
    } else {
      if (read_count < 0) {
        std::cerr << "error in GetEvents: " << SDL_GetError() << std::endl;
      }
      return;
    }
  }
}

WeakPointer<Engine2Impl> Engine2Impl::GetWeakPointer() {
  WeakPointer<Engine2Impl> weak;
  weak_factory_.GetWeakPointer(&weak);
  return weak;
}

std::unique_ptr<Engine2::RunClause> Engine2Impl::EveryFrame() {
  return std::make_unique<EveryFrameRunClause>(GetWeakPointer());
}

std::unique_ptr<Engine2::KeyboardEventClause> Engine2Impl::OnKey(
    SDL_Keycode key_code) {
  return std::make_unique<KeyboardEventClauseImpl>(GetWeakPointer(), key_code);
}

std::unique_ptr<Engine2::KeyboardEventClause> Engine2Impl::OnKey(
    const std::string& name) {
  return OnKey(SDL_GetKeyFromName(name.c_str()));
}

void Engine2Impl::SetKeyDownHandler(SDL_Keycode key_code,
                                    KeyboardCallback callback) {
  event_handlers_.key_down_.insert_or_assign(key_code, callback);
}

void Engine2Impl::SetKeyUpHandler(SDL_Keycode key_code,
                                  KeyboardCallback callback) {
  event_handlers_.key_up_.insert_or_assign(key_code, callback);
}

}  // namespace engine2