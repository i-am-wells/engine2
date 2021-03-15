#include "engine2/ui/image_view.h"

namespace engine2 {
namespace ui {

ImageView::ImageView(Texture* texture,
                     Graphics2D* graphics,
                     const Rect<>& source_rect,
                     double scale,
                     const Vec<int, 2>& padding,
                     const Vec<int, 2>& margin)
    : HierarchyView(padding, margin),
      texture_(texture),
      graphics_(graphics),
      source_rect_(source_rect),
      scale_(scale) {}

Vec<int, 2> ImageView::GetSize() const {
  return GetScaledTextureSize() + (GetPadding() * 2);
}

Vec<int64_t, 2> ImageView::GetScaledTextureSize() const {
  return texture_->GetSize().size * Vec<double, 2>::Fill(scale_);
}

void ImageView::Draw() const {
  graphics_->DrawTexture(*texture_, source_rect_,
                         {GetAbsoluteInnerPosition(), GetScaledTextureSize()});
}

}  // namespace ui
}  // namespace engine2