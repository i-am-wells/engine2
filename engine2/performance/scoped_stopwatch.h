#ifndef ENGINE2_PERFORMACE_SCOPED_STOPWATCH_H_
#define ENGINE2_PERFORMACE_SCOPED_STOPWATCH_H_

#include "engine2/performance/perf_span.h"
#include "engine2/time.h"

namespace engine2 {

class ScopedStopwatch {
 public:
  ScopedStopwatch(PerfSpan::Id span_id);
  ~ScopedStopwatch();

 private:
  PerfSpan span_;
};

}  // namespace engine2

#ifdef PERF
#define TIME_THIS_SCOPE_AS(id) ScopedStopwatch __scoped_stopwatch(id)
#else
#define TIME_THIS_SCOPE_AS(id)
#endif

#endif  // ENGINE2_PERFORMACE_SCOPED_STOPWATCH_H_