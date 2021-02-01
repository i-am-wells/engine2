#include "tools/tilemapeditor/sidebar_view.h"
#include "tools/tilemapeditor/editor.h"

using engine2::Font;
using engine2::Graphics2D;
using engine2::kOpaque;
using engine2::Point;
using engine2::Rect;
using engine2::RgbaColor;
using engine2::Vec;
using engine2::ui::ListView;
using engine2::ui::View;

using engine2::kBlack;
using engine2::kGray;
using engine2::kRed;

namespace tilemapeditor {

SidebarView::SidebarView(Editor* editor)
    : ListView(Direction::kVertical),
      editor_(editor),
      tile_picker_widget_(editor, "Tile picker"),
      layer_picker_widget_(editor, "Layers") {
  relative_pos_ = {5, 5};
  AddChildren({&tile_picker_widget_, &layer_picker_widget_});
}

void SidebarView::Draw() const {
  editor_->graphics()->SetDrawColor(kGray)->FillRect(
      GetRect().template ConvertTo<int64_t>());

  ListView::Draw();
}

template <class ViewType>
SidebarView::Widget<ViewType>::Widget(Editor* editor, const std::string& title)
    : ListView(Direction::kVertical, GetPadding()),
      editor_(editor),
      title_(editor->graphics(), editor->font(), title, kBlack),
      inner_view_(editor) {
  AddChildren({&title_, &inner_view_});
}

template <class ViewType>
void SidebarView::Widget<ViewType>::Draw() const {
  editor_->graphics()->SetDrawColor(kBlack)->DrawRect(
      GetRect().template ConvertTo<int64_t>());
  ListView::Draw();
}

template <class ViewType>
Vec<int, 2> SidebarView::Widget<ViewType>::GetMargin() const {
  return {5, 5};
}

template <class ViewType>
Vec<int, 2> SidebarView::Widget<ViewType>::GetPadding() const {
  return {10, 10};
}

}  // namespace tilemapeditor