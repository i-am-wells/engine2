#include "engine2/ui/text_view.h"

namespace engine2 {
namespace ui {

TextView::TextView(Graphics2D* graphics,
                   Font* font,
                   const std::string& text,
                   RgbaColor color)
    : graphics_(graphics), font_(font), color_(color) {
  RenderText(text);
}

void TextView::SetPosition(const Point<int, 2>& position) {
  rect_.pos = position;
}

Rect<int, 2> TextView::GetRect() const {
  return rect_;
}

void TextView::Draw() const {
  if (texture_)
    graphics_->DrawTexture(*texture_, rect_.template ConvertTo<int64_t>());
}

void TextView::RenderText(const std::string& text) {
  texture_ = font_->Render(graphics_, text, color_);
  if (texture_)
    rect_ = {rect_.pos, texture_->GetSize().size.template ConvertTo<int>()};
}

}  // namespace ui
}  // namespace engine2