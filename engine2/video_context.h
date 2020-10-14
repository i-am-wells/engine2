#ifndef ENGINE2_VIDEO_CONTEXT_H_
#define ENGINE2_VIDEO_CONTEXT_H_

#include <memory>

#include "engine2/state_mutex.h"
#include "engine2/types.h"

namespace engine2 {

class VideoContext {
 public:
  static std::unique_ptr<VideoContext> Create();

  class EveryFrameClause {
   public:
    virtual EveryFrameClause* OnReadState(Callback read_state_callback) = 0;
    virtual EveryFrameClause* OnDraw(Callback draw_callback) = 0;
    virtual ~EveryFrameClause() = default;
  };

  virtual std::unique_ptr<EveryFrameClause> EveryFrame() = 0;
  virtual void Run(StateMutex* state_mutex) = 0;

  virtual ~VideoContext() = default;
};

}  // namespace engine2

#endif  // ENGINE2_VIDEO_CONTEXT_H_