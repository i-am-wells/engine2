#ifndef TOOLS_TILEMAPEDITOR_TILE_PICKER_H_
#define TOOLS_TILEMAPEDITOR_TILE_PICKER_H_

#include <map>
#include <set>

#include "engine2/sprite.h"
#include "engine2/sprite_cache.h"
#include "engine2/texture.h"
#include "engine2/ui/container_view.h"
#include "tools/tilemapeditor/two_finger_touch.h"

namespace tilemapeditor {

class Editor;

class TilePicker : public engine2::ui::ContainerView {
 public:
  TilePicker(Editor* editor, engine2::SpriteCache* sprite_cache);

  void Init();

  void Draw() const override;

  void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;
  void OnMouseButtonUp(const SDL_MouseButtonEvent& event) override;
  void OnMouseMotion(const SDL_MouseMotionEvent& event) override;

  void OnFingerDown(const SDL_TouchFingerEvent& event);
  void OnFingerUp(const SDL_TouchFingerEvent& event);
  void OnFingerMotion(const SDL_TouchFingerEvent& event);

  // TODO remove
  uint16_t GetSelectedTileIndex() const;

  // Apply selection to the map.
  void CopyToMap(const engine2::Point<>& map_point,
                 int layer,
                 bool new_stroke = true);

  engine2::Vec<int, 2> GetSize() const override;

 private:
  void UpdateSelectedSprites();

  engine2::Vec<int64_t, 2> ScaledSize() const;
  engine2::Vec<int64_t, 2> ScaledTileSize() const;

  engine2::Point<> ScreenToImage(const engine2::Point<>& screen_point) const;
  engine2::Point<> ImageToScreen(const engine2::Point<>& image_point) const;
  engine2::Point<> ScreenToGrid(const engine2::Point<>& screen_point) const;

  double scale_ = 4.;

  engine2::SpriteCache* sprite_cache_;
  engine2::Texture* tiles_image_;
  Editor* editor_;
  engine2::Point<> grid_size_;
  engine2::ui::TextView sprite_sheet_name_view_;

  std::map<std::string, uint16_t> sprite_name_to_map_index_;
  std::set<engine2::Sprite*> selected_sprites_;

  // TODO still needed?
  std::string selected_sprite_name_;
  engine2::Sprite* selected_sprite_ = nullptr;

  // Tile coords.
  RectangleSelection selection_;
  bool selecting_ = false;

  class TwoFingerHandler : public TwoFingerTouch::Handler {
   public:
    TwoFingerHandler(TilePicker* picker);
    void OnPinch(const engine2::Point<double, 2>& center,
                 double pinch_factor) override;
    void OnDrag(const engine2::Vec<double, 2>& drag_amount) override;

   private:
    TilePicker* picker_;
  };
  TwoFingerHandler two_finger_handler_;
  TwoFingerTouch two_finger_touch_;
  friend class TwoFingerHandler;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_TILE_PICKER_H_