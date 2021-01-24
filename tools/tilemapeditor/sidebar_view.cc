#include "tools/tilemapeditor/sidebar_view.h"

using engine2::Font;
using engine2::Graphics2D;
using engine2::kOpaque;
using engine2::Point;
using engine2::Rect;
using engine2::RgbaColor;
using engine2::ui::View;

static constexpr RgbaColor kBlack{0, 0, 0, kOpaque};
static constexpr RgbaColor kGray{128, 128, 128, kOpaque};

namespace tilemapeditor {

SidebarView::SidebarView(const Rect<int, 2>& rect,
                         Graphics2D* graphics,
                         Font* font)
    : ContainerView(rect, {&text_view_}),
      graphics_(graphics),
      text_view_(graphics, font, "some text", kGray) {
  text_view_.SetPosition(rect_.pos + Point<int, 2>{10, 10});
}

void SidebarView::Draw() const {
  graphics_->SetDrawColor(kBlack)->FillRect(
      rect_.template ConvertTo<int64_t>());
  ContainerView::Draw();
}

}  // namespace tilemapeditor