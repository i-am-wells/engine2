#ifndef TOOLS_TILEMAPEDITOR_EDITOR_H_
#define TOOLS_TILEMAPEDITOR_EDITOR_H_

#include "engine2/event_handler.h"
#include "engine2/frame_loop.h"
#include "engine2/graphics2d.h"
#include "engine2/tile_map.h"

namespace tilemapeditor {

class Editor : public engine2::FrameLoop, public engine2::EventHandler {
 public:
  Editor(std::unique_ptr<engine2::Graphics2D> graphics, engine2::TileMap* map);

  // FrameLoop
  void EveryFrame() override;

  // EventHandler
  void OnKeyDown(const SDL_KeyboardEvent& event) override;
  void OnKeyUp(const SDL_KeyboardEvent& event) override;

  void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;
  void OnMouseButtonUp(const SDL_MouseButtonEvent& event) override;
  void OnMouseMotion(const SDL_MouseMotionEvent& event) override;
  void OnMouseWheel(const SDL_MouseWheelEvent& event) override;

 private:
  std::unique_ptr<engine2::Graphics2D> graphics_;
  engine2::TileMap* map_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_EDITOR_H_