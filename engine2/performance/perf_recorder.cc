#include "engine2/performance/perf_recorder.h"

namespace engine2 {

#ifdef PERF
PerfRecorder gPerfRecorder;
#endif

void PerfRecorder::RecordSpan(PerfSpan perf_span) {
  buffer_.push_back(perf_span);
}

void PerfRecorder::AddListener(Listener* listener) {
  listeners_.push_back(listener);
}

// Call all listeners' OnFlush and clear buffer_.
void PerfRecorder::Flush() {
  for (auto* listener : listeners_)
    listener->OnFlush(buffer_);
  buffer_.clear();
}

}  // namespace engine2