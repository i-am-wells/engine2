#include <algorithm>
#include <cmath>
#include <iostream>

#include "engine2/rgba_color.h"
#include "tools/tilemapeditor/editor.h"

using engine2::Font;
using engine2::Graphics2D;
using engine2::kOpaque;
using engine2::Point;
using engine2::Rect;
using engine2::RgbaColor;
using engine2::SpriteCache;
using engine2::Texture;
using engine2::TileMap;
using engine2::Vec;
using engine2::Window;

using engine2::kBlack;
using engine2::kDarkGray;
using engine2::kGray;
using engine2::kGreen;
using engine2::kRed;
using engine2::kWhite;

using engine2::ui::ImageView;

namespace tilemapeditor {
namespace {

static constexpr Vec<int64_t, 2> kNorth{0, -1};
static constexpr Vec<int64_t, 2> kSouth{0, 1};
static constexpr Vec<int64_t, 2> kEast{1, 0};
static constexpr Vec<int64_t, 2> kWest{-1, 0};

static constexpr Vec<int64_t, 2> kIconSize{8, 8};
static constexpr double kIconScale = 4.;

static constexpr int64_t kSpeed = 2;

template <class T>
void Print(const std::string& msg, const T& val) {
  std::cerr << msg << " " << val.x() << " " << val.y() << '\n';
}

Rect<> IconRect(const Point<>& p) {
  return {p * kIconSize, kIconSize};
}

}  // namespace

Editor::Editor(Window* window,
               Graphics2D* graphics,
               Font* font,
               TileMap* map,
               Texture* icons_image,
               SpriteCache* sprite_cache)
    : FrameLoop(/*event_handler=*/this),
      window_(window),
      graphics_(graphics),
      font_(font),
      world_graphics_(graphics_, &(window_in_world_.pos)),
      map_(map),
      tile_picker_(this, sprite_cache),
      two_finger_handler_(this),
      two_finger_touch_(&two_finger_handler_),
      tool_buttons_(this, icons_image) {
  window_in_world_.pos = {};
  window_in_world_.size = graphics_->GetSize().size;

  // TODO get correct display (and account for display origin!)
  display_size_ = window_->GetDisplaySize().ConvertTo<double>();

  tool_buttons_.SetRelativePosition({10, int(display_size_.y()) - 800});
}

void Editor::Init() {
  tile_picker_.Init();

  // TODO do something different
  // Set grid randomly
  TileMap::GridPoint point;
  for (point.y() = 0; point.y() < 50; ++point.y()) {
    for (point.x() = 0; point.x() < 50; ++point.x()) {
      map_->SetTileIndex(point, /*layer=*/0, (rand() % 4) + 1);
      map_->SetTileIndex(point, 1, 0);
    }
  }
}

Point<int64_t, 2> Editor::TouchPointToPixels(
    const Point<double, 2>& touch_point) const {
  Point<double, 2> display_point = touch_point * display_size_;

  return display_point.ConvertTo<int64_t>();
  //- window_->GetInnerPosition().ConvertTo<int64_t>();
}

Vec<int64_t, 2> Editor::TouchMotionToPixels(
    const Vec<double, 2>& touch_motion) const {
  return (touch_motion * display_size_).ConvertTo<int64_t>();
}

void Editor::EveryFrame() {
  graphics_->SetDrawColor(kDarkGray)->Clear();
  map_->Draw(graphics_, window_in_world_);
  // DrawMapGrid();
  DrawSelectionHighlight();
  DrawCursorHighlight();

  // sidebar_.Draw();
  tile_picker_.Draw();

  tool_buttons_.Draw();

  window_in_world_.pos += viewport_velocity_;

  graphics_->Present();
  framerate_regulator_.Wait();
}

void Editor::OnKeyDown(const SDL_KeyboardEvent& event) {
  if (event.repeat)
    return;

  switch (event.keysym.sym) {
    case SDLK_w:
      viewport_velocity_ += kNorth * kSpeed;
      break;
    case SDLK_a:
      viewport_velocity_ += kWest * kSpeed;
      break;
    case SDLK_s:
      viewport_velocity_ += kSouth * kSpeed;
      break;
    case SDLK_d:
      viewport_velocity_ += kEast * kSpeed;
      break;
    // TODO: remove once layer picker is done!
    case SDLK_0:
      layer_ = 0;
      break;
    case SDLK_1:
      layer_ = 1;
      break;
    case SDLK_ESCAPE:
      Stop();
      break;
    default:
      break;
  }
}

void Editor::OnKeyUp(const SDL_KeyboardEvent& event) {
  if (event.repeat)
    return;

  switch (event.keysym.sym) {
    case SDLK_w:
      viewport_velocity_ -= kNorth * kSpeed;
      break;
    case SDLK_a:
      viewport_velocity_ -= kWest * kSpeed;
      break;
    case SDLK_s:
      viewport_velocity_ -= kSouth * kSpeed;
      break;
    case SDLK_d:
      viewport_velocity_ -= kEast * kSpeed;
      break;
    default:
      break;
  }
}

void Editor::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
  Point<int, 2> point{event.x, event.y};
  if (tile_picker_.Contains(point))
    return tile_picker_.OnMouseButtonDown(event);

  if (tool_buttons_.Contains(point))
    return tool_buttons_.OnMouseButtonDown(event);

  if (event.which != SDL_TOUCH_MOUSEID) {
    SetCursorGridPosition({event.x, event.y});
    map_->SetTileIndex(last_cursor_map_position_, layer_,
                       tile_picker_.GetSelectedTileIndex());
    mouse_down_ = true;
  }
}

void Editor::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {
  // if (sidebar_.Contains({event.x, event.y})) {
  //  sidebar_.OnMouseButtonUp(event);
  //}
  mouse_down_ = false;
}

void Editor::OnMouseMotion(const SDL_MouseMotionEvent& event) {
  // if (sidebar_.Contains({event.x, event.y})) {
  //  sidebar_.OnMouseMotion(event);
  //}
  if (tile_picker_.Contains({event.x, event.y}))
    return;

  SetCursorGridPosition({event.x, event.y});
  if (mouse_down_) {
    map_->SetTileIndex(last_cursor_map_position_, layer_,
                       tile_picker_.GetSelectedTileIndex());
  }
}

void Editor::OnMouseWheel(const SDL_MouseWheelEvent& event) {
  // TODO try to pass to UI; if it isn't on any UI, pass to map
}

void Editor::OnFingerDown(const SDL_TouchFingerEvent& event) {
  Point<> point = TouchPointToPixels({event.x, event.y});
  if (tile_picker_.Contains(point.ConvertTo<int>())) {
    tile_picker_.OnFingerDown(event);
  } else {
    two_finger_touch_.OnFingerDown(event);
  }
}

void Editor::OnFingerUp(const SDL_TouchFingerEvent& event) {
  tile_picker_.OnFingerUp(event);
  two_finger_touch_.OnFingerUp(event);
}

void Editor::OnFingerMotion(const SDL_TouchFingerEvent& event) {
  tile_picker_.OnFingerMotion(event);
  two_finger_touch_.OnFingerMotion(event);
}

void Editor::DrawMapGrid() {
  Vec<int64_t, 2> grid_size_pixels_ = grid_size_tiles_ * tile_size_;
  Vec<int64_t, 2> phase = window_in_world_.pos % grid_size_pixels_;
  graphics_->SetDrawColor(kGreen);

  grid_size_pixels_.x() *= scale_;
  grid_size_pixels_.y() *= scale_;
  phase.x() *= scale_;
  phase.y() *= scale_;

  // Draw vertical lines
  for (int64_t x = window_in_world_.x() - phase.x();
       x < window_in_world_.x() + window_in_world_.w();
       x += grid_size_pixels_.x()) {
    if (x == 0)
      graphics_->SetDrawColor(kRed);

    world_graphics_.DrawLine(
        {x, window_in_world_.y()},
        {x, int64_t(window_in_world_.y() + window_in_world_.h() * scale_)});
    if (x == 0)
      graphics_->SetDrawColor(kGreen);
  }

  // Draw horizontal lines
  for (int64_t y = window_in_world_.y() - phase.y();
       y < window_in_world_.y() + window_in_world_.h();
       y += grid_size_pixels_.y()) {
    if (y == 0)
      graphics_->SetDrawColor(kRed);

    world_graphics_.DrawLine(
        {window_in_world_.x(), y},
        {int64_t(window_in_world_.x() + window_in_world_.w() * scale_), y});
    if (y == 0)
      graphics_->SetDrawColor(kGreen);
  }
}

void Editor::DrawCursorHighlight() {
  graphics_->SetDrawColor(kGreen);

  Vec<int64_t, 2> size =
      (tile_size_.ConvertTo<double>() * scale_).ConvertTo<int64_t>();
  Point<> pos = WorldToScreen(map_->GridToWorld(last_cursor_map_position_));

  graphics_->DrawRect({pos, size});
}

void Editor::DrawSelectionHighlight() {
  graphics_->SetDrawColor(kGreen);
  world_graphics_.DrawRect(map_selection_ * tile_size_);
}

void Editor::SetCursorGridPosition(const Point<>& screen_pos) {
  last_cursor_map_position_ = map_->WorldToGrid(ScreenToWorld(screen_pos));
}

Point<> Editor::ScreenToWorld(const Point<>& pixel_point) const {
  return (pixel_point.ConvertTo<double>() / scale_).ConvertTo<int64_t>() +
         window_in_world_.pos;
}

Point<> Editor::WorldToScreen(const Point<>& world_point) const {
  return ((world_point - window_in_world_.pos).ConvertTo<double>() * scale_)
      .ConvertTo<int64_t>();
}

Vec<int64_t, 2> Editor::GetGraphicsLogicalSize() const {
  // return graphics_->GetLogicalSize().size;
  return graphics_->GetSize().size;
}

Editor::TwoFingerHandler::TwoFingerHandler(Editor* editor) : editor_(editor) {}

void Editor::TwoFingerHandler::OnPinch(const Point<double, 2>& center,
                                       double pinch_factor) {
  editor_->scale_ *= pinch_factor;
  editor_->map_->SetScale(editor_->scale_);

  // TODO: center zoom on center!
  Vec<int64_t, 2> old_size = editor_->window_in_world_.size;
  editor_->window_in_world_.size =
      (editor_->GetGraphicsLogicalSize().ConvertTo<double>() / editor_->scale_)
          .ConvertTo<int64_t>();

  editor_->window_in_world_.pos -=
      ((editor_->window_in_world_.size - old_size).ConvertTo<double>() * center)
          .ConvertTo<int64_t>();
}

void Editor::TwoFingerHandler::OnDrag(const Vec<double, 2>& drag_amount) {
  editor_->window_in_world_.pos -=
      editor_->TouchMotionToPixels(drag_amount / editor_->scale_);
}

Editor::ToolButton::ToolButton(ToolButtonTray* tray,
                               Texture* icons,
                               Graphics2D* graphics,
                               const Rect<>& source_rect,
                               double scale,
                               ToolMode mode)
    : ImageView(icons, graphics, source_rect, scale),
      tray_(tray),
      mode_(mode) {}

void Editor::ToolButton::Draw() const {
  if (selected_)
    graphics_->SetDrawColor(kRed);
  else
    graphics_->SetDrawColor(kWhite);

  graphics_->FillRect(GetRect().ConvertTo<int64_t>());
  ImageView::Draw();
}
Vec<int, 2> Editor::ToolButton::GetMargin() const {
  return {20, 20};
}
Vec<int, 2> Editor::ToolButton::GetPadding() const {
  return {8, 8};
}

void Editor::ToolButton::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
  tray_->Select(this);
}

void Editor::ToolButton::SetSelected(bool selected) {
  selected_ = selected;
}
bool Editor::ToolButton::IsSelected() const {
  return selected_;
}

Editor::ToolButtonTray::ToolButtonTray(Editor* editor, Texture* icons)
    : engine2::ui::ListView(Direction::kVertical),
      editor_(editor),
      draw_(this,
            icons,
            editor->graphics_,
            IconRect({1, 0}),
            kIconScale,
            ToolMode::kDraw),
      erase_(this,
             icons,
             editor->graphics_,
             IconRect({1, 1}),
             kIconScale,
             ToolMode::kErase),
      paste_(this,
             icons,
             editor->graphics_,
             IconRect({2, 3}),
             kIconScale,
             ToolMode::kPaste),
      select_(this,
              icons,
              editor->graphics_,
              IconRect({2, 0}),
              kIconScale,
              ToolMode::kSelect) {
  AddChildren({&select_, &paste_, &erase_, &draw_});
  Select(&draw_);
}

void Editor::ToolButtonTray::Init() {}

void Editor::ToolButtonTray::Select(ToolButton* button) {
  if (selected_)
    selected_->SetSelected(false);

  selected_ = button;
  editor_->tool_mode_ = button->mode();
  selected_->SetSelected(true);
}

}  // namespace tilemapeditor