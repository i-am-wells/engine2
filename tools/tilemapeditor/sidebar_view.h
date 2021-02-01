#ifndef TOOLS_TILEMAPEDITOR_SIDEBAR_VIEW_H_
#define TOOLS_TILEMAPEDITOR_SIDEBAR_VIEW_H_

#include <vector>

#include "engine2/ui/list_view.h"
#include "engine2/ui/text_view.h"

#include "tools/tilemapeditor/layer_picker.h"
#include "tools/tilemapeditor/tile_picker.h"

namespace tilemapeditor {

class Editor;

class SidebarView : public engine2::ui::ListView {
 public:
  SidebarView(Editor* editor);

  void Draw() const override;

 private:
  template <class ViewType>
  class Widget : public engine2::ui::ListView {
   public:
    Widget(Editor* editor, const std::string& title);
    void Draw() const override;

    engine2::Vec<int, 2> GetMargin() const override;
    engine2::Vec<int, 2> GetPadding() const override;

   protected:
    Editor* editor_;
    engine2::ui::TextView title_;
    ViewType inner_view_;
  };

  Editor* editor_;
  Widget<TilePicker> tile_picker_widget_;
  Widget<LayerPicker> layer_picker_widget_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_SIDEBAR_VIEW_H_