#ifndef TOOLS_TILEMAPEDITOR_EDITOR_H_
#define TOOLS_TILEMAPEDITOR_EDITOR_H_

#include "engine2/event_handler.h"
#include "engine2/font.h"
#include "engine2/frame_loop.h"
#include "engine2/graphics2d.h"
#include "engine2/offset_graphics2d.h"
#include "engine2/sprite_cache.h"
#include "engine2/tile_map.h"
#include "engine2/timing.h"
#include "engine2/ui/container_view.h"
#include "engine2/ui/image_view.h"
#include "engine2/ui/list_view.h"
#include "engine2/ui/text_view.h"
#include "engine2/window.h"

#include "tools/tilemapeditor/action_stack.h"
#include "tools/tilemapeditor/rectangle_selection.h"
#include "tools/tilemapeditor/sidebar_view.h"
#include "tools/tilemapeditor/two_finger_touch.h"

namespace tilemapeditor {

class Editor : public engine2::FrameLoop, public engine2::EventHandler {
 public:
  Editor(engine2::Window* window,
         engine2::Graphics2D* graphics,
         engine2::Font* font,
         engine2::TileMap* map,
         engine2::Texture* icons_image,
         engine2::SpriteCache* sprite_cache,
         const std::string& file_path,
         const std::string& initial_status_text);

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

  void OnQuit(const SDL_QuitEvent& event) override;

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

  // Editor actions
  void Redo();
  void Undo();
  void Save();
  void SetSingleTileIndex(const engine2::TileMap::GridPoint& point,
                          int layer,
                          uint16_t index,
                          ActionStack* action_stack,
                          bool new_stroke = true);
  void SetSingleTileIndexInternal(const engine2::TileMap::GridPoint& point,
                                  int layer,
                                  uint16_t index,
                                  ActionStack* action_stack);

  void FloodFill(const engine2::TileMap::GridPoint& point,
                 int layer,
                 uint16_t index,
                 ActionStack* action_stack);

  void StartMove(const engine2::Point<> world_point);
  void ContinueMove(const engine2::Point<> world_point);
  void CancelMove();
  void FinishMove(const engine2::Point<> world_point);

  void SetTilesInRect(const engine2::Rect<>& rect,
                      uint16_t index,
                      ActionStack* action_stack);
  void CopyTiles(engine2::TileMap* source,
                 const engine2::Point<>& source_pos,
                 int source_layer,
                 engine2::TileMap* dest,
                 const engine2::Point<>& dest_pos,
                 int dest_layer,
                 const engine2::Vec<int64_t, 2>& size,
                 ActionStack* action_stack);

  void UndoRedoInternal(ActionStack* stack,
                        ActionStack* anti_stack,
                        const std::string& undid_or_redid);
  void SetStatusText(const std::string& status);
  void Error(const std::string& message);

  engine2::Window* window_;
  engine2::Graphics2D* graphics_;
  engine2::Font* font_;
  engine2::OffsetGraphics2D world_graphics_;
  engine2::TileMap* map_;
  std::string file_path_;
  engine2::SpriteCache* sprite_cache_;
  engine2::Timing::FramerateRegulator framerate_regulator_{60};

  // SidebarView sidebar_;
  TilePicker tile_picker_;
  friend class TilePicker;

  engine2::Rect<> window_in_world_;
  engine2::Vec<int, 2> window_inner_size_;
  engine2::Vec<int64_t, 2> grid_size_tiles_{10, 10};  // save
  engine2::TileMap::GridPoint last_cursor_map_position_{};
  RectangleSelection map_selection_;
  int layer_ = 0;
  engine2::Vec<double, 2> display_size_{};
  engine2::Vec<double, 2> scale_{1., 1.};
  bool mouse_down_ = false;
  ActionStack undo_stack_, redo_stack_;
  bool show_flags_ = true;

  std::unique_ptr<engine2::TileMap> move_buffer_;
  engine2::Point<> move_cursor_offset_tiles_{};
  engine2::Point<> move_cursor_offset_{};
  engine2::Rect<> move_map_original_rect_{};

  bool map_mouse_drag_ = false;
  engine2::Point<int, 2> map_mouse_drag_screen_start_{};
  engine2::Point<> map_mouse_drag_world_point_{};

  engine2::ui::TextView status_bar_;

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
    kMove,
  };

  class ToolButtonTray;
  class ToolButton : public engine2::ui::ListView {
   public:
    ToolButton(ToolButtonTray* tray,
               const engine2::Rect<>& source_rect,
               ToolMode mode,
               const std::string& name);

    void Init();
    void Draw() const override;
    void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;

    void SetSelected(bool selected);
    bool IsSelected() const;

    ToolMode mode() const { return mode_; }

   private:
    bool selected_ = false;
    ToolButtonTray* tray_;
    ToolMode mode_;
    engine2::ui::ImageView icon_view_;
    engine2::ui::TextView name_view_;
  };

  class ToolButtonTray : public engine2::ui::ListView {
   public:
    ToolButtonTray(Editor* editor, engine2::Texture* icons);
    void Init();

    void Select(ToolButton* button);

   private:
    friend class ToolButton;
    Editor* editor_;
    engine2::Texture* icons_;
    ToolButton buttons_[6];
    ToolButton* selected_ = nullptr;
  };
  ToolButtonTray tool_buttons_;
  ToolMode tool_mode_;

  class TileMapObserver : public engine2::TileMap::Observer {
   public:
    TileMapObserver(Editor* editor);
    void OnDrawTile(engine2::TileMap::Tile* tile,
                    const engine2::Rect<int, 2>& screen_rect) override;

   private:
    Editor* editor_;
  };
  TileMapObserver tile_map_observer_{this};
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_EDITOR_H_