#include <iostream>

#include "tools/tilemapeditor/editor.h"
#include "tools/tilemapeditor/tile_picker.h"

using engine2::Point;
using engine2::Rect;
using engine2::Sprite;
using engine2::SpriteCache;
using engine2::Texture;
using engine2::Vec;

namespace tilemapeditor {

TilePicker::TilePicker(Editor* editor, SpriteCache* sprite_cache)
    : ContainerView(/*padding=*/Vec<int, 2>{20, 20}),
      editor_(editor),
      sprite_cache_(sprite_cache),
      sprite_sheet_name_view_(editor_->graphics(),
                              editor_->font(),
                              "",
                              engine2::kBlack,
                              Vec<int, 2>{10, 10}),
      two_finger_handler_(this),
      two_finger_touch_(&two_finger_handler_) {
  // TODO: support multiple sprite sheets
  // for now, assume there's just one
  for (auto& [key, sprite] : *sprite_cache_) {
    tiles_image_ = sprite.texture();
    break;
  }
}

void TilePicker::Init() {
  SetSize(ScaledSize() + (padding_ * 2));

  Vec<int64_t, 2> image_size = tiles_image_->GetSize().size;
  grid_size_ = image_size / editor_->map_->GetTileSize();

  Vec<int, 2> picker_size = GetSize();
  SetRelativePosition(
      {int(editor_->graphics()->GetSize().size.x()) - picker_size.x(), 0});

  sprite_sheet_name_view_.SetParent(this);
  sprite_sheet_name_view_.SetRelativePosition({0, picker_size.y()});
  sprite_sheet_name_view_.SetScale({3, 3});

  // Get all tiles and add to map
  for (auto& [path, sprite] : *sprite_cache_) {
    sprite_name_to_map_index_.emplace(path, editor_->map_->GetTileCount());
    editor_->map_->AddTile({&sprite});
  }
}

void TilePicker::Draw() const {
  engine2::Graphics2D* graphics = editor_->graphics();
  graphics->SetDrawColor(engine2::kRed)->FillRect(GetRect());

  Point<int, 2> inner_pos = GetAbsoluteInnerPosition();
  graphics->DrawTexture(*tiles_image_, {inner_pos, ScaledSize()});

  graphics->SetDrawColor(engine2::kRed)
      ->FillRect(sprite_sheet_name_view_.GetRect());
  sprite_sheet_name_view_.Draw();

  if (selected_sprite_) {
    Rect<> sel_rect =
        selected_sprite_->Frame(0).source_rect * Vec<double, 2>::Fill(scale_);
    editor_->graphics()->DrawRect({inner_pos + sel_rect.pos, sel_rect.size});
  }
}

void TilePicker::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
  Point<> point_in_image = Point<>{event.x, event.y} - GetRect().pos - padding_;
  point_in_image /= scale_;

  for (auto& [path, sprite] : *sprite_cache_) {
    if (sprite.Frame(0).source_rect.Contains(point_in_image)) {
      selected_sprite_name_ = path;
      selected_sprite_ = &sprite;

      sprite_sheet_name_view_.SetText(path);
    }
  }
}

void TilePicker::OnFingerDown(const SDL_TouchFingerEvent& event) {
  two_finger_touch_.OnFingerDown(event);
}
void TilePicker::OnFingerUp(const SDL_TouchFingerEvent& event) {
  two_finger_touch_.OnFingerUp(event);
}
void TilePicker::OnFingerMotion(const SDL_TouchFingerEvent& event) {
  two_finger_touch_.OnFingerMotion(event);
}

uint16_t TilePicker::GetSelectedTileIndex() const {
  if (selected_sprite_name_.empty())
    return 0;

  auto iter = sprite_name_to_map_index_.find(selected_sprite_name_);
  if (iter == sprite_name_to_map_index_.end())
    return 0;

  return iter->second;
}

Vec<int, 2> TilePicker::GetSize() const {
  return ScaledSize() + padding_ * 2;
}

Vec<int64_t, 2> TilePicker::ScaledSize() const {
  return tiles_image_->GetSize().size * Vec<double, 2>::Fill(scale_);
}

Vec<int64_t, 2> TilePicker::ScaledTileSize() const {
  return editor_->map_->GetTileSize() * Vec<double, 2>::Fill(scale_);
}

TilePicker::TwoFingerHandler::TwoFingerHandler(TilePicker* picker)
    : picker_(picker) {}

void TilePicker::TwoFingerHandler::OnPinch(
    const engine2::Point<double, 2>& center,
    double pinch_factor) {
  picker_->scale_ *= pinch_factor;
  picker_->sprite_sheet_name_view_.SetRelativePosition(
      {0, picker_->GetSize().y()});
}

void TilePicker::TwoFingerHandler::OnDrag(
    const engine2::Vec<double, 2>& drag_amount) {
  picker_->SetRelativePosition(
      picker_->GetRelativePosition() +
      picker_->editor_->TouchMotionToPixels(drag_amount));
}

}  // namespace tilemapeditor