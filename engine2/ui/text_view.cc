#include "engine2/ui/text_view.h"

namespace engine2 {
namespace ui {

TextView::TextView(Graphics2D* graphics,
                   Font* font,
                   const std::string& text,
                   RgbaColor color,
                   const Vec<int, 2>& padding)
    : graphics_(graphics), font_(font), color_(color) {
  RenderText(text, padding);
}

void TextView::Draw() const {
  Rect<int, 2> draw_rect{GetAbsolutePosition() + GetPadding(), texture_size_};
  if (texture_)
    graphics_->DrawTexture(*texture_, draw_rect.template ConvertTo<int64_t>());
}

void TextView::RenderText(const std::string& text) {
  RenderText(text, GetPadding());
}

void TextView::RenderText(const std::string& text, const Vec<int, 2>& padding) {
  text_ = text;
  texture_ = font_->Render(graphics_, text, color_);

  // Re-rendering text may change view size.
  if (texture_) {
    texture_size_ = texture_->GetSize().size.template ConvertTo<int>();
    size_ = texture_size_ + (padding * 2);
  } else {
    size_ = {};
    texture_size_ = {};
  }

  OnSizeChanged();
}

std::string TextView::GetText() const {
  return text_;
}

void TextView::SetText(const std::string& text) {
  RenderText(text, GetPadding());
}

}  // namespace ui
}  // namespace engine2