#ifndef ENGINE2_IMPL_VIDEO_CONTEXT_IMPL_H_
#define ENGINE2_IMPL_VIDEO_CONTEXT_IMPL_H_

#include <optional>

#include "engine2/memory/weak_pointer.h"
#include "engine2/video_context.h"

namespace engine2 {

class VideoContextImpl : public VideoContext {
 public:
  VideoContextImpl();
  ~VideoContextImpl() override;

  std::unique_ptr<EveryFrameClause> EveryFrame() override;
  void Run(StateMutex* state_mutex) override;

  void SetReadStateCallback(Callback read_state_callback);
  void SetDrawCallback(Callback draw_callback);

 private:
  std::optional<Callback> read_state_callback_;
  std::optional<Callback> draw_callback_;
  WeakPointer<VideoContextImpl>::Factory weak_factory_{this};
};

}  // namespace engine2

#endif  // ENGINE2_IMPL_VIDEO_CONTEXT_IMPL_H_