#ifndef TOOLS_TILEMAPEDITOR_LAYER_PICKER_H_
#define TOOLS_TILEMAPEDITOR_LAYER_PICKER_H_

#include "engine2/ui/container_view.h"

namespace tilemapeditor {

class Editor;

class LayerPicker : public engine2::ui::ContainerView {
 public:
  LayerPicker(Editor* editor);

  void Draw() const override;

 private:
  Editor* editor_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_LAYER_PICKER_H_