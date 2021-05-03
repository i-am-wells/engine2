#include "engine2/performance/scoped_stopwatch.h"
#include "engine2/performance/perf_recorder.h"

namespace engine2 {

ScopedStopwatch::ScopedStopwatch(PerfSpan::Id span_id)
    : span_{span_id, Time::Now(), {}} {}

ScopedStopwatch::~ScopedStopwatch() {
  span_.end = Time::Now();
  gPerfRecorder.RecordSpan(span_);
}

}  // namespace engine2