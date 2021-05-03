#include "engine2/performance/frame_stats_overlay.h"

namespace engine2 {
namespace {

const int kLeftMargin = 5;
const int kLineHeight = 20;

void NewLine(Point<int, 2>* point) {
  point->x() = kLeftMargin;
  point->y() += kLineHeight;
}

}  // namespace

FrameStatsOverlay::FrameStatsOverlay(Graphics2D* graphics,
                                     Font* font,
                                     std::vector<PerfSpan::Id> spans_to_display)
    : span_ids_(std::move(spans_to_display)), graphics_(graphics) {
  gPerfRecorder.AddListener(this);
  Reset();

  // Pre-render all text
  fps_texture_ = font->Render(graphics, " fps", kBlack);
  ms_texture_ = font->Render(graphics, " ms", kBlack);
  for (int i = 0; i < 10; ++i)
    number_textures_[i] = font->Render(graphics, std::to_string(i), kBlack);

  for (PerfSpan::Id id : span_ids_) {
    name_textures_.push_back(
        font->Render(graphics, PerfSpanIdName(id) + ": ", kBlack));
  }
}

void FrameStatsOverlay::Draw() {
  Point<int, 2> draw_point{kLeftMargin, 5};

  int64_t frame_avg_duration_us =
      id_to_summary_[PerfSpan::Id::kFrame].duration_avg.ToMicroseconds();
  if (!frame_avg_duration_us)
    frame_avg_duration_us = -1'000'000;

  DrawNumber(1'000'000 / frame_avg_duration_us, &draw_point);
  DrawTexture(*fps_texture_, &draw_point);

  int i = 0;
  for (PerfSpan::Id id : span_ids_) {
    NewLine(&draw_point);
    DrawTexture(*name_textures_[i], &draw_point);
    DrawNumber(AvgDurationMs(id), &draw_point);
    DrawTexture(*ms_texture_, &draw_point);
    ++i;
  }
}

void FrameStatsOverlay::OnFlush(const std::vector<PerfSpan>& buffer) {
  Reset();

  for (const PerfSpan& span : buffer) {
    SpanSummary& summary = id_to_summary_[span.id];
    ++summary.call_count;
    summary.duration_sum += (span.end - span.begin);
  }

  for (auto& [id, summary] : id_to_summary_) {
    if (summary.call_count)
      summary.duration_avg = summary.duration_sum / summary.call_count;
    else
      summary.duration_avg = Time::Delta::FromSeconds(-1);
  }
}

int FrameStatsOverlay::AvgDurationMs(PerfSpan::Id id) const {
  return id_to_summary_.find(id)->second.duration_avg.ToMicroseconds() / 1000;
}

void FrameStatsOverlay::DrawTexture(const Texture& texture,
                                    Point<int, 2>* draw_point) const {
  Vec<int, 2> size = texture.GetSize().size;
  graphics_->DrawTexture(texture, {*draw_point, size});
  draw_point->x() += size.x();
}

void FrameStatsOverlay::DrawNumber(int num, Point<int, 2>* draw_point) const {
  // TODO fix for negative numbers
  if (num < 0)
    return;
  while (num) {
    DrawTexture(*number_textures_[num % 10], draw_point);
    num /= 10;
  }
}

void FrameStatsOverlay::Reset() {
  for (PerfSpan::Id id : span_ids_)
    id_to_summary_[id] = SpanSummary();
}

}  // namespace engine2