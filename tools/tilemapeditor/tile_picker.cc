#include <SDL2/SDL_mouse.h>
#include <iostream>

#include "tools/tilemapeditor/editor.h"
#include "tools/tilemapeditor/tile_picker.h"

using engine2::Point;
using engine2::Rect;
using engine2::Sprite;
using engine2::SpriteCache;
using engine2::Texture;
using engine2::TileMap;
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
  // TODO which sprite is selected at the beginning?
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
  for (int i = 0; i < editor_->map_->GetTileCount(); ++i) {
    TileMap::Tile* tile = editor_->map_->GetTileByIndex(i);
    sprite_to_map_index_[tile->sprite] = i;
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

  Rect<> sel_rect = selection_.GetRect();
  Point<> tile_size = editor_->map_->GetTileSize();
  editor_->graphics()->DrawRect({ImageToScreen(sel_rect.pos * tile_size),
                                 sel_rect.size * ScaledTileSize()});

  for (Sprite* sprite : selected_sprites_) {
    Rect<>& sprite_rect = sprite->Frame(0).source_rect;
    editor_->graphics()->DrawRect(
        {ImageToScreen(sprite_rect.pos),
         sprite_rect.size * Vec<int64_t, 2>::Fill(scale_)});
  }
}

void TilePicker::CopyToMap(const Point<>& map_point,
                           int layer,
                           bool new_stroke) {
  bool first_tile = true;
  for (Sprite* sprite : selected_sprites_) {
    uint16_t index = sprite_to_map_index_[sprite];

    Rect<>& sprite_rect = sprite->Frame(0).source_rect;
    Point<> draw_point = map_point +
                         (sprite_rect.pos / editor_->map_->GetTileSize()) -
                         selection_.GetRect().pos;

    editor_->SetSingleTileIndex({draw_point.x(), draw_point.y()}, layer, index,
                                &editor_->undo_stack_,
                                new_stroke && first_tile);
    if (first_tile)
      first_tile = false;
  }
}

Point<> TilePicker::ScreenToImage(const Point<>& screen_point) const {
  Point<> point_in_image = screen_point - GetAbsoluteInnerPosition();
  point_in_image /= scale_;
  return point_in_image;
}

Point<> TilePicker::ImageToScreen(const Point<>& image_point) const {
  Point<> screen_point = image_point;
  screen_point *= scale_;
  return screen_point + GetAbsoluteInnerPosition();
}

Point<> TilePicker::ScreenToGrid(const Point<>& screen_point) const {
  return ScreenToImage(screen_point) / editor_->map_->GetTileSize();
}

void TilePicker::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
  Point<int, 2> point{event.x, event.y};
  if (event.button == SDL_BUTTON_RIGHT) {
    mouse_drag_ = true;
    mouse_drag_screen_pos_ = point;
    return;
  }

  selection_.Start(ScreenToGrid(point));
  selecting_ = true;

  UpdateSelectedSprites();
}

void TilePicker::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {
  if (event.button == SDL_BUTTON_RIGHT) {
    mouse_drag_ = false;
    return;
  }

  if (selecting_) {
    selection_.Update(ScreenToGrid({event.x, event.y}));
    UpdateSelectedSprites();
    selecting_ = false;
  }
}

void TilePicker::OnMouseMotion(const SDL_MouseMotionEvent& event) {
  Point<int, 2> point{event.x, event.y};
  if (mouse_drag_) {
    Vec<double, 2> drag_amount = point - mouse_drag_screen_pos_;
    two_finger_handler_.OnDrag(drag_amount /
                               editor_->graphics()->GetSize().size);
    mouse_drag_screen_pos_ = point;
    return;
  }

  if (selecting_) {
    selection_.Update(ScreenToGrid(point));
    UpdateSelectedSprites();
  }
}

void TilePicker::OnMouseWheel(const SDL_MouseWheelEvent& event) {
  double zoom_amount = 0;
  if (event.y > 0)
    zoom_amount = 0.8;
  else if (event.y < 0)
    zoom_amount = 1.25;

  Point<int, 2> point;
  SDL_GetMouseState(&point.x(), &point.y());
  two_finger_handler_.OnPinch(point / editor_->graphics()->GetSize().size,
                              zoom_amount);
}

void TilePicker::UpdateSelectedSprites() {
  Rect<> selection_in_image =
      selection_.GetRect() * editor_->map_->GetTileSize();

  // TODO avoid iterating over entire sprite cache!
  selected_sprites_.clear();
  for (auto& [path, sprite] : *sprite_cache_) {
    if (sprite.Frame(0).source_rect.Overlaps(selection_in_image)) {
      // selected_sprite_name_ = path;
      // sprite_sheet_name_view_.SetText(path);

      selected_sprites_.insert(&sprite);
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
  if (selected_sprites_.empty())
    return 0;
  auto iter = sprite_to_map_index_.find(*selected_sprites_.cbegin());
  if (iter == sprite_to_map_index_.end())
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