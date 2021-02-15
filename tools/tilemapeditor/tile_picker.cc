#include "tools/tilemapeditor/tile_picker.h"
#include "tools/tilemapeditor/editor.h"

using engine2::Point;
using engine2::Rect;
using engine2::Sprite;
using engine2::Texture;
using engine2::Vec;

namespace tilemapeditor {

TilePicker::TilePicker(Editor* editor, Texture* tiles_image)
    : editor_(editor), tiles_image_(tiles_image) {
  Vec<int64_t, 2> size = ScaledSize() + (padding_ * 2l);
  SetSize(size.ConvertTo<int>());
}

void TilePicker::Init() {
  Vec<int64_t, 2> image_size = tiles_image_->GetSize().size;
  grid_size_ = image_size / editor_->tile_size_;

  // Get all tiles and add to map
  Point<> p;
  for (p.y() = 0; p.y() < image_size.y(); p.y() += editor_->tile_size_.y()) {
    for (p.x() = 0; p.x() < image_size.x(); p.x() += editor_->tile_size_.x()) {
      sprites_.emplace_front(tiles_image_, Rect<>{p, editor_->tile_size_});

      picker_index_to_map_index_.insert(
          {PickerIndex(p), editor_->map_->GetTileCount()});

      editor_->map_->AddTile({&(sprites_.front())});
    }
  }
}

void TilePicker::Draw() const {
  editor_->graphics()
      ->SetDrawColor(engine2::kRed)
      ->FillRect(GetRect().ConvertTo<int64_t>());

  editor_->graphics()->DrawTexture(*tiles_image_, {padding_, ScaledSize()});

  Vec<int64_t, 2> scaled_tile_size = ScaledTileSize();
  Point<> select_grid_point{selected_picker_index_ % grid_size_.x(),
                            selected_picker_index_ / grid_size_.x()};
  editor_->graphics()->DrawRect(
      {select_grid_point * scaled_tile_size + padding_, scaled_tile_size});
}

void TilePicker::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
  Point<> point_in_image =
      Point<>{event.x, event.y} - GetRect().pos.ConvertTo<int64_t>() - padding_;
  selected_picker_index_ = PickerIndex(
      (point_in_image.ConvertTo<double>() / scale_).ConvertTo<int64_t>());
}

uint16_t TilePicker::GetSelectedTileIndex() const {
  if (selected_picker_index_ < 0)
    return 0;

  auto iter = picker_index_to_map_index_.find(selected_picker_index_);
  if (iter == picker_index_to_map_index_.end())
    return 0;

  return iter->second;
}

Vec<int64_t, 2> TilePicker::ScaledSize() const {
  return (tiles_image_->GetSize().size.ConvertTo<double>() * scale_)
      .ConvertTo<int64_t>();
}

Vec<int64_t, 2> TilePicker::ScaledTileSize() const {
  return (editor_->tile_size_.ConvertTo<double>() * scale_)
      .ConvertTo<int64_t>();
}

int TilePicker::PickerIndex(const Point<>& image_point) const {
  Point<> grid_point = image_point / editor_->tile_size_;
  return grid_point.y() * grid_size_.x() + grid_point.x();
}

}  // namespace tilemapeditor