#ifndef ENGINE2_PERFORMANCE_FRAME_STATS_OVERLAY_H_
#define ENGINE2_PERFORMANCE_FRAME_STATS_OVERLAY_H_

#include <array>
#include <map>

#include "engine2/font.h"
#include "engine2/graphics2d.h"
#include "engine2/performance/perf_recorder.h"
#include "engine2/performance/perf_span.h"
#include "engine2/time.h"

namespace engine2 {

class FrameStatsOverlay : public PerfRecorder::Listener {
 public:
  FrameStatsOverlay(Graphics2D* graphics,
                    Font* font,
                    std::vector<PerfSpan::Id> spans_to_display);

  void Draw();

  // PerfRecorder::Listener
  void OnFlush(const std::vector<PerfSpan>& buffer) override;

 private:
  int AvgDurationMs(PerfSpan::Id id) const;

  void DrawTexture(const Texture& texture, Point<int, 2>* draw_point) const;
  void DrawNumber(int num, Point<int, 2>* draw_point) const;

  void Reset();

  std::vector<PerfSpan::Id> span_ids_;

  struct SpanSummary {
    int call_count = 0;
    Time::Delta duration_sum{};
    Time::Delta duration_avg{};
  };
  std::map<PerfSpan::Id, SpanSummary> id_to_summary_;

  Graphics2D* graphics_;

  std::unique_ptr<Texture> fps_texture_;
  std::array<std::unique_ptr<Texture>, 10> number_textures_;
  std::unique_ptr<Texture> ms_texture_;
  std::vector<std::unique_ptr<Texture>> name_textures_;
};

}  // namespace engine2

#endif  // ENGINE2_PERFORMANCE_FRAME_STATS_OVERLAY_H_