#include "engine2/impl/logic_context_impl.h"

#include <functional>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

namespace engine2 {
namespace {

void StripKeyboardEvent(Callback callback, const SDL_KeyboardEvent& event) {
  callback();
}

class EveryFrameCancelable : public LogicContext::Cancelable {
 public:
  EveryFrameCancelable(WeakPointer<LogicContextImpl> context, uint32_t id)
      : context_(std::move(context)), id_(id) {}
  void Cancel() override {
    if (!context_)
      return;
    context_->CancelEveryFrameCallback(id_);
  }
  ~EveryFrameCancelable() override = default;

 private:
  WeakPointer<LogicContextImpl> context_;
  uint32_t id_;
};

class EveryFrameRunClause : public LogicContext::RunClause {
 public:
  explicit EveryFrameRunClause(WeakPointer<LogicContextImpl> context)
      : context_(std::move(context)) {}
  std::unique_ptr<LogicContext::Cancelable> Run(Callback callback) override {
    uint32_t id = 0;
    if (context_)
      id = context_->AddEveryFrameCallback(std::move(callback));

    return std::make_unique<EveryFrameCancelable>(context_, id);
  }
  ~EveryFrameRunClause() override = default;

 private:
  WeakPointer<LogicContextImpl> context_;
};

class KeyboardEventClauseImpl : public LogicContext::KeyboardEventClause {
 public:
  explicit KeyboardEventClauseImpl(WeakPointer<LogicContextImpl> context,
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

  WeakPointer<LogicContextImpl> context_;
  SDL_Keycode key_code_;
  WeakPointer<KeyboardEventClause>::Factory weak_factory_{this};
};

class ScheduledCancelable : public LogicContext::Cancelable {
 public:
  ScheduledCancelable(WeakPointer<LogicContextImpl> context,
                      uint32_t callback_id)
      : context_(std::move(context)), callback_id_(callback_id) {}
  void Cancel() override {
    if (context_)
      context_->CancelScheduledCallback(callback_id_);
  }

 private:
  WeakPointer<LogicContextImpl> context_;
  uint32_t callback_id_;
};

class ScheduledRunClause : public LogicContext::RunClause {
 public:
  ScheduledRunClause(WeakPointer<LogicContextImpl> context,
                     double num,
                     Timing::TimeUnit unit,
                     bool repeat)
      : context_(std::move(context)), num_(num), unit_(unit), repeat_(repeat) {}
  ~ScheduledRunClause() override = default;
  std::unique_ptr<LogicContext::Cancelable> Run(Callback callback) override {
    uint32_t id = -1;
    if (context_) {
      id =
          context_->ScheduleCallback(std::move(callback), num_, unit_, repeat_);
    }

    return std::make_unique<ScheduledCancelable>(context_, id);
  }

 private:
  WeakPointer<LogicContextImpl> context_;
  double num_;
  Timing::TimeUnit unit_;
  bool repeat_;
};

}  // namespace

LogicContextImpl::LogicContextImpl() {
  // TODO SDL inits here
}

LogicContextImpl::~LogicContextImpl() {
  // TODO SDL quits here
}

void LogicContextImpl::Run(StateMutex* state_mutex) {
  if (running_)
    return;

  running_ = true;
  while (running_) {
    {
      auto lock = std::move(state_mutex->Lock());

      // Run all EveryFrame() callbacks
      for (auto& callback : every_frame_callbacks_)
        callback.callback();

      // Run all timed callbacks
      callback_queue_.RunCurrent();

      HandleSDLEvents();

      state_mutex->SendSignal(std::move(lock), StateMutex::Signal::kGo);
    }

    framerate_regulator_.Wait();
  }
  // in case we exited the loop with break
  running_ = false;
}

void LogicContextImpl::Stop() {
  running_ = false;
}

void LogicContextImpl::HandleSDLEvents() {
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

WeakPointer<LogicContextImpl> LogicContextImpl::GetWeakPointer() {
  WeakPointer<LogicContextImpl> weak;
  weak_factory_.GetWeakPointer(&weak);
  return weak;
}

std::unique_ptr<LogicContext::RunClause> LogicContextImpl::EveryFrame() {
  return std::make_unique<EveryFrameRunClause>(GetWeakPointer());
}

std::unique_ptr<LogicContext::RunClause> LogicContextImpl::Every(
    double num,
    Timing::TimeUnit unit) {
  return std::make_unique<ScheduledRunClause>(GetWeakPointer(), num, unit,
                                              /*repeat=*/true);
}

std::unique_ptr<LogicContext::RunClause> LogicContextImpl::After(
    double num,
    Timing::TimeUnit unit) {
  return std::make_unique<ScheduledRunClause>(GetWeakPointer(), num, unit,
                                              /*repeat=*/false);
}

std::unique_ptr<LogicContext::KeyboardEventClause> LogicContextImpl::OnKey(
    SDL_Keycode key_code) {
  return std::make_unique<KeyboardEventClauseImpl>(GetWeakPointer(), key_code);
}

std::unique_ptr<LogicContext::KeyboardEventClause> LogicContextImpl::OnKey(
    const std::string& name) {
  return OnKey(SDL_GetKeyFromName(name.c_str()));
}

uint32_t LogicContextImpl::AddEveryFrameCallback(Callback callback) {
  every_frame_callbacks_.push_back({callback, ++next_every_frame_callback_id_});
  return next_every_frame_callback_id_;
}

void LogicContextImpl::CancelEveryFrameCallback(uint32_t id) {
  int i = 0;
  for (auto& callback : every_frame_callbacks_) {
    if (callback.id == id) {
      every_frame_callbacks_.erase(every_frame_callbacks_.begin() + i);
      return;
    }
    ++i;
  }
}

uint32_t LogicContextImpl::ScheduleCallback(Callback callback,
                                            double num,
                                            Timing::TimeUnit unit,
                                            bool repeat) {
  return callback_queue_.Schedule(std::move(callback), num, unit, repeat);
}

void LogicContextImpl::CancelScheduledCallback(uint32_t id) {
  callback_queue_.Cancel(id);
}

void LogicContextImpl::SetKeyDownHandler(SDL_Keycode key_code,
                                         KeyboardCallback callback) {
  event_handlers_.key_down_.insert_or_assign(key_code, callback);
}

void LogicContextImpl::SetKeyUpHandler(SDL_Keycode key_code,
                                       KeyboardCallback callback) {
  event_handlers_.key_up_.insert_or_assign(key_code, callback);
}

}  // namespace engine2