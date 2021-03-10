#ifndef TOOLS_TILEMAPEDITOR_TILE_PICKER_H_
#define TOOLS_TILEMAPEDITOR_TILE_PICKER_H_

#include <list>
#include <map>

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

  void OnFingerDown(const SDL_TouchFingerEvent& event);
  void OnFingerUp(const SDL_TouchFingerEvent& event);
  void OnFingerMotion(const SDL_TouchFingerEvent& event);

  uint16_t GetSelectedTileIndex() const;

  engine2::Vec<int, 2> GetSize() const override;

 private:
  engine2::Vec<int64_t, 2> ScaledSize() const;
  engine2::Vec<int64_t, 2> ScaledTileSize() const;

  engine2::Vec<int64_t, 2> padding_{20, 20};
  double scale_ = 8.;

  engine2::SpriteCache* sprite_cache_;
  engine2::Texture* tiles_image_;
  Editor* editor_;
  engine2::Point<> grid_size_;

  std::map<std::string, uint16_t> sprite_name_to_map_index_;
  std::string selected_sprite_name_;
  engine2::Sprite* selected_sprite_ = nullptr;

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