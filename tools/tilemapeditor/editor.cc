#include "tools/tilemapeditor/editor.h"

using engine2::Graphics2D;
using engine2::TileMap;

namespace tilemapeditor {

Editor::Editor(std::unique_ptr<Graphics2D> graphics, TileMap* map)
    : FrameLoop(/*event_handler=*/this),
      graphics_(std::move(graphics)),
      map_(map) {}

void Editor::EveryFrame() {
  graphics_->Clear();
  map_->Draw(graphics_.get(), {0, 0, 800, 600});
  graphics_->Present();
  // TODO delay for framerate?
}

void Editor::OnKeyDown(const SDL_KeyboardEvent& event) {
  switch (event.keysym.sym) {
    case SDLK_w:
      // TODO up
      break;
    case SDLK_a:
      break;
    case SDLK_s:
      break;
    case SDLK_d:
      break;
    default:
  }
}

void Editor::OnKeyUp(const SDL_KeyboardEvent& event) {
  switch (event.keysym.sym) {
    case SDLK_w:
      // TODO up
      break;
    case SDLK_a:
      break;
    case SDLK_s:
      break;
    case SDLK_d:
      break;
    default:
  }
}

void Editor::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {}
void Editor::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {}
void Editor::OnMouseMotion(const SDL_MouseMotionEvent& event) {}
void Editor::OnMouseWheel(const SDL_MouseWheelEvent& event) {}

}  // namespace tilemapeditor