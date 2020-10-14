#include "engine2/video_context.h"
#include "engine2/impl/video_context_impl.h"

namespace engine2 {

// static
std::unique_ptr<VideoContext> VideoContext::Create() {
  return std::make_unique<VideoContextImpl>();
}

}  // namespace engine2