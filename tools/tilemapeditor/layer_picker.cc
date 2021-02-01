#include "tools/tilemapeditor/layer_picker.h"
#include "tools/tilemapeditor/editor.h"

namespace tilemapeditor {

LayerPicker::LayerPicker(Editor* editor) : editor_(editor) {
  SetSize({80, 80});
}

void LayerPicker::Draw() const {
  editor_->graphics()
      ->SetDrawColor(engine2::kBlack)
      ->FillRect(GetRect().template ConvertTo<int64_t>());
}

}  // namespace tilemapeditor