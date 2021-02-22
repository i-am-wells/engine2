#ifndef TOOLS_TILEMAPEDITOR_EDITOR_H_
#define TOOLS_TILEMAPEDITOR_EDITOR_H_

#include "engine2/event_handler.h"
#include "engine2/font.h"
#include "engine2/frame_loop.h"
#include "engine2/graphics2d.h"
#include "engine2/tile_map.h"
#include "engine2/timing.h"
#include "engine2/ui/image_view.h"
#include "engine2/ui/list_view.h"
#include "engine2/window.h"

#include "tools/tilemapeditor/sidebar_view.h"
#include "tools/tilemapeditor/two_finger_touch.h"

namespace tilemapeditor {

class Editor : public engine2::FrameLoop, public engine2::EventHandler {
 public:
  Editor(engine2::Window* window,
         engine2::Graphics2D* graphics,
         engine2::Font* font,
         engine2::TileMap* map,
         engine2::Texture* tiles_image,
         engine2::Texture* icons_image);

  engine2::Vec<int, 2> TileSize() const {
    return tile_size_.template ConvertTo<int>();
  }

  void Init();

  // FrameLoop
  void EveryFrame() override;

  // EventHandler
  void OnKeyDown(const SDL_KeyboardEvent& event) override;
  void OnKeyUp(const SDL_KeyboardEvent& event) override;

  void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;
  void OnMouseButtonUp(const SDL_MouseButtonEvent& event) override;
  void OnMouseMotion(const SDL_MouseMotionEvent& event) override;
  void OnMouseWheel(const SDL_MouseWheelEvent& event) override;

  void OnFingerDown(const SDL_TouchFingerEvent& event) override;
  void OnFingerUp(const SDL_TouchFingerEvent& event) override;
  void OnFingerMotion(const SDL_TouchFingerEvent& event) override;

  engine2::Graphics2D* graphics() { return graphics_; }
  engine2::Font* font() { return font_; }
  engine2::Point<int64_t, 2> TouchPointToPixels(
      const engine2::Point<double, 2>& touch_point) const;
  engine2::Vec<int64_t, 2> TouchMotionToPixels(
      const engine2::Vec<double, 2>& touch_motion) const;

 private:
  void DrawMapGrid();
  void DrawCursorHighlight();
  void DrawSelectionHighlight();

  void SetCursorGridPosition(const engine2::Point<>& screen_pos);

  engine2::Point<> ScreenToWorld(const engine2::Point<>& pixel_point) const;
  engine2::Point<> WorldToScreen(const engine2::Point<>& world_point) const;

  engine2::Vec<int64_t, 2> GetGraphicsLogicalSize() const;

  engine2::Window* window_;
  engine2::Graphics2D* graphics_;
  engine2::Font* font_;
  engine2::OffsetGraphics2D world_graphics_;
  engine2::TileMap* map_;
  engine2::Rect<> window_size_;
  engine2::Timing::FramerateRegulator framerate_regulator_{60};

  // SidebarView sidebar_;
  TilePicker tile_picker_;
  friend class TilePicker;

  engine2::Rect<> window_in_world_;
  engine2::Vec<int64_t, 2> viewport_velocity_{};
  engine2::Vec<int64_t, 2> tile_size_{16, 16};
  engine2::Vec<int64_t, 2> grid_size_tiles_{10, 10};  // save
  engine2::TileMap::GridPoint last_cursor_map_position_{};
  engine2::Rect<> map_selection_{};

  engine2::Vec<double, 2> display_size_{};

  double scale_ = 1.;

  bool mouse_down_ = false;

  class TwoFingerHandler : public TwoFingerTouch::Handler {
   public:
    TwoFingerHandler(Editor* editor);
    void OnPinch(const engine2::Point<double, 2>& center,
                 double pinch_factor) override;
    void OnDrag(const engine2::Vec<double, 2>& drag_amount) override;

   private:
    Editor* editor_;
  };
  friend class TwoFingerHandler;
  TwoFingerHandler two_finger_handler_;
  TwoFingerTouch two_finger_touch_;

  enum class ToolMode {
    kDraw,
    kErase,
    kSelect,
    kPaste,
    kFill,
  };

  class ToolButtonTray;
  class ToolButton : public engine2::ui::ImageView {
   public:
    ToolButton(ToolButtonTray* tray,
               engine2::Texture* icons,
               engine2::Graphics2D* graphics,
               const engine2::Rect<>& source_rect,
               double scale,
               ToolMode mode);

    void Draw() const override;
    engine2::Vec<int, 2> GetMargin() const override;
    engine2::Vec<int, 2> GetPadding() const override;

    void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;

    void SetSelected(bool selected);
    bool IsSelected() const;

    ToolMode mode() const { return mode_; }

   private:
    bool selected_ = false;
    ToolButtonTray* tray_;
    ToolMode mode_;
  };

  class ToolButtonTray : public engine2::ui::ListView {
   public:
    ToolButtonTray(Editor* editor, engine2::Texture* icons);
    void Init();

    void Select(ToolButton* button);

   private:
    Editor* editor_;
    ToolButton draw_;
    ToolButton erase_;
    ToolButton paste_;
    ToolButton select_;
    ToolButton* selected_ = nullptr;
  };
  ToolButtonTray tool_buttons_;
  ToolMode tool_mode_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_EDITOR_H_