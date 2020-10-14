#include "engine2/impl/video_context_impl.h"

namespace engine2 {
namespace {

class EveryFrameClauseImpl : public VideoContext::EveryFrameClause {
 public:
  EveryFrameClauseImpl(WeakPointer<VideoContextImpl> context)
      : context_(std::move(context)) {}
  ~EveryFrameClauseImpl() = default;
  EveryFrameClause* OnReadState(Callback read_state_callback) override {
    if (context_)
      context_->SetReadStateCallback(read_state_callback);
    return this;
  }
  EveryFrameClause* OnDraw(Callback draw_callback) override {
    if (context_)
      context_->SetDrawCallback(draw_callback);
    return this;
  }

 private:
  WeakPointer<VideoContextImpl> context_;
};

}  // namespace

VideoContextImpl::VideoContextImpl() {
  // TODO throw exception
  SDL_VideoInit(nullptr);
}

VideoContextImpl::~VideoContextImpl() {
  SDL_VideoQuit();
}

std::unique_ptr<VideoContext::EveryFrameClause> VideoContextImpl::EveryFrame() {
  WeakPointer<VideoContextImpl> weak;
  weak_factory_.GetWeakPointer(&weak);
  return std::make_unique<EveryFrameClauseImpl>(std::move(weak));
}

void VideoContextImpl::Run(StateMutex* state_mutex) {
  // TODO: should this loop work more like the one in LogicContextImpl::Run?
  while (true) {
    {
      StateMutex::WaitAndLockResult lock =
          std::move(state_mutex->WaitAndLock());
      if (lock.signal == StateMutex::Signal::kQuit)
        break;

      // Here because it must happen on the video thread with state locked.
      SDL_PumpEvents();

      if (read_state_callback_)
        (*read_state_callback_)();
    }

    if (draw_callback_)
      (*draw_callback_)();
  }
}

void VideoContextImpl::SetReadStateCallback(Callback read_state_callback) {
  read_state_callback_ = read_state_callback;
}

void VideoContextImpl::SetDrawCallback(Callback draw_callback) {
  draw_callback_ = draw_callback;
}

}  // namespace engine2