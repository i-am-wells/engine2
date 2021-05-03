#include "engine2/performance/perf_span.h"

namespace engine2 {

std::string PerfSpanIdName(PerfSpan::Id id) {
  switch (id) {
    case PerfSpan::Id::kFrame:
      return "Frame";
    case PerfSpan::Id::kFrameIdle:
      return "Frame idle";
    case PerfSpan::Id::kDraw:
      return "Draw";
    case PerfSpan::Id::kTileMapDraw:
      return "Tile map draw";
    case PerfSpan::Id::kCameraObserve:
      return "Camera observe";
    case PerfSpan::Id::kCameraDraw:
      return "Camera draw";
    case PerfSpan::Id::kPerfOverlayDraw:
      return "Perf overlay draw";
    case PerfSpan::Id::kRenderPresent:
      return "SDL render present";
    case PerfSpan::Id::kSpaceAdvanceTime:
      return "Space advance time";
    case PerfSpan::Id::kHandleEvents:
      return "Handle events";
  }
}

}  // namespace engine2