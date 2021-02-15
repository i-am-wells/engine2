#ifndef TOOLS_TILEMAPEDITOR_TILE_PICKER_H_
#define TOOLS_TILEMAPEDITOR_TILE_PICKER_H_

#include <list>
#include <map>

#include "engine2/sprite.h"
#include "engine2/texture.h"
#include "engine2/ui/container_view.h"

namespace tilemapeditor {

class Editor;

class TilePicker : public engine2::ui::ContainerView {
 public:
  TilePicker(Editor* editor, engine2::Texture* tiles_image);

  void Init();

  void Draw() const override;

  void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;

  uint16_t GetSelectedTileIndex() const;

 private:
  engine2::Vec<int64_t, 2> ScaledSize() const;
  engine2::Vec<int64_t, 2> ScaledTileSize() const;
  int PickerIndex(const engine2::Point<>& image_point) const;

  engine2::Vec<int64_t, 2> padding_{20, 20};
  double scale_ = 4.;

  engine2::Texture* tiles_image_;
  Editor* editor_;
  engine2::Point<> grid_size_;

  std::list<engine2::Sprite> sprites_;
  std::map<int, uint16_t> picker_index_to_map_index_;

  int selected_picker_index_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_TILE_PICKER_H_