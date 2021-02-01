#ifndef TOOLS_TILEMAPEDITOR_EDITOR_H_
#define TOOLS_TILEMAPEDITOR_EDITOR_H_

#include "engine2/event_handler.h"
#include "engine2/font.h"
#include "engine2/frame_loop.h"
#include "engine2/graphics2d.h"
#include "engine2/tile_map.h"
#include "engine2/timing.h"
#include "engine2/window.h"
#include "tools/tilemapeditor/sidebar_view.h"

namespace tilemapeditor {

class Editor : public engine2::FrameLoop, public engine2::EventHandler {
 public:
  Editor(engine2::Window* window,
         engine2::Graphics2D* graphics,
         engine2::Font* font,
         engine2::TileMap* map);

  engine2::Vec<int, 2> TileSize() const {
    return tile_size_.template ConvertTo<int>();
  }

  // FrameLoop
  void EveryFrame() override;

  // EventHandler
  void OnKeyDown(const SDL_KeyboardEvent& event) override;
  void OnKeyUp(const SDL_KeyboardEvent& event) override;

  void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;
  void OnMouseButtonUp(const SDL_MouseButtonEvent& event) override;
  void OnMouseMotion(const SDL_MouseMotionEvent& event) override;
  void OnMouseWheel(const SDL_MouseWheelEvent& event) override;

  engine2::Graphics2D* graphics() { return graphics_; }
  engine2::Font* font() { return font_; }

 private:
  void DrawMapGrid();
  void DrawCursorHighlight();
  void DrawSelectionHighlight();

  void SetCursorGridPosition(const engine2::Point<>& screen_pos);

  engine2::Window* window_;
  engine2::Graphics2D* graphics_;
  engine2::Font* font_;
  engine2::OffsetGraphics2D world_graphics_;
  engine2::TileMap* map_;
  engine2::Rect<> window_size_;
  engine2::Timing::FramerateRegulator framerate_regulator_{60};

  SidebarView sidebar_;

  engine2::Rect<> window_in_world_;
  engine2::Vec<int64_t, 2> viewport_velocity_{};
  engine2::Vec<int64_t, 2> tile_size_{16, 16};
  engine2::Vec<int64_t, 2> grid_size_tiles_{10, 10};  // save
  engine2::TileMap::GridPoint last_cursor_map_position_{};
  engine2::Rect<> map_selection_{};
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_EDITOR_H_