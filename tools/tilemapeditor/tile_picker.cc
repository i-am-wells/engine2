#include "tools/tilemapeditor/tile_picker.h"
#include "tools/tilemapeditor/editor.h"

namespace tilemapeditor {

TilePicker::TilePicker(Editor* editor) : editor_(editor) {
  // TODO add child views here
  SetSize({80, 80});
}

void TilePicker::Draw() const {
  editor_->graphics()
      ->SetDrawColor(engine2::kRed)
      ->FillRect(GetRect().template ConvertTo<int64_t>());
}

}  // namespace tilemapeditor