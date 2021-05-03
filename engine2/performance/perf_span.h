#ifndef ENGINE2_PERFORMANCE_PERF_SPAN_H_
#define ENGINE2_PERFORMANCE_PERF_SPAN_H_

#include <string>

#include "engine2/time.h"

namespace engine2 {

struct PerfSpan {
  enum class Id {
    kFrame,
    kFrameIdle,

    kDraw,
    kTileMapDraw,
    kCameraObserve,
    kCameraDraw,
    kPerfOverlayDraw,
    kRenderPresent,
    kSpaceAdvanceTime,

    kHandleEvents,
  };

  Id id;
  Time begin;
  Time end;
};

std::string PerfSpanIdName(PerfSpan::Id id);

}  // namespace engine2

#endif  // ENGINE2_PERFORMANCE_PERF_SPAN_H_