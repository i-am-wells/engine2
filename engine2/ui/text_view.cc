#include "engine2/ui/text_view.h"

namespace engine2 {
namespace ui {

TextView::TextView(Graphics2D* graphics,
                   Font* font,
                   const std::string& text,
                   RgbaColor color,
                   const Vec<int, 2>& padding,
                   const Vec<int, 2>& margin)
    : HierarchyView(padding, margin),
      graphics_(graphics),
      font_(font),
      color_(color),
      text_(text) {}

void TextView::Init() {
  RenderText();
}

void TextView::Draw() const {
  Rect<int, 2> draw_rect{GetAbsoluteInnerPosition(), inner_size_ * scale_};
  if (texture_)
    graphics_->DrawTexture(*texture_, draw_rect);
}

Vec<int, 2> TextView::GetSize() const {
  return (inner_size_ * scale_) + (GetPadding() * 2);
}

void TextView::RenderText() {
  texture_ = font_->Render(graphics_, text_, color_);

  // Re-rendering text may change view size.
  if (texture_)
    inner_size_ = texture_->GetSize().size;
  else
    inner_size_ = {};

  OnSizeChanged();
}

std::string TextView::GetText() const {
  return text_;
}

void TextView::SetText(const std::string& text) {
  text_ = text;
  RenderText();
}

}  // namespace ui
}  // namespace engine2