#ifndef ENGINE2_PERFORMANCE_PERF_RECORDER_H_
#define ENGINE2_PERFORMANCE_PERF_RECORDER_H_

#include <vector>

#include "engine2/performance/perf_span.h"
#include "engine2/time.h"

namespace engine2 {

class PerfRecorder {
 public:
  void RecordSpan(PerfSpan span);

  class Listener {
   public:
    virtual void OnFlush(const std::vector<PerfSpan>& buffer) = 0;
  };
  void AddListener(Listener* listener);

  // Call all listeners' OnFlush and clear buffer_.
  void Flush();

 private:
  std::vector<PerfSpan> buffer_;
  std::vector<Listener*> listeners_;
};

#ifdef PERF
extern PerfRecorder gPerfRecorder;
#endif

}  // namespace engine2

#endif  // ENGINE2_PERFORMANCE_PERF_RECORDER_H_