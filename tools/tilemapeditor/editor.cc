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
using engine2::TileMap;
using engine2::Vec;
using engine2::Window;

using engine2::kBlack;
using engine2::kDarkGray;
using engine2::kGray;
using engine2::kGreen;
using engine2::kRed;

namespace tilemapeditor {
namespace {

static constexpr Vec<int64_t, 2> kNorth{0, -1};
static constexpr Vec<int64_t, 2> kSouth{0, 1};
static constexpr Vec<int64_t, 2> kEast{1, 0};
static constexpr Vec<int64_t, 2> kWest{-1, 0};

static constexpr int64_t kSpeed = 2;

template <class T>
void Print(const std::string& msg, const T& val) {
  std::cerr << msg << " " << val.x() << " " << val.y() << '\n';
}

}  // namespace

Editor::Editor(Window* window, Graphics2D* graphics, Font* font, TileMap* map)
    : FrameLoop(/*event_handler=*/this),
      window_(window),
      graphics_(graphics),
      font_(font),
      world_graphics_(graphics_, &(window_in_world_.pos)),
      map_(map),
      sidebar_(this),
      two_finger_handler_(this),
      two_finger_touch_(&two_finger_handler_) {
  window_in_world_.pos = {};
  window_in_world_.size = graphics_->GetLogicalSize().size;

  // TODO get correct display (and account for display origin!)
  display_size_ = window_->GetDisplaySize().ConvertTo<double>();
}

Point<int64_t, 2> Editor::TouchPointToPixels(
    const Point<double, 2>& touch_point) const {
  Point<double, 2> display_point = touch_point * display_size_;

  return display_point.ConvertTo<int64_t>() -
         window_->GetInnerPosition().ConvertTo<int64_t>();
}

Vec<int64_t, 2> Editor::TouchMotionToPixels(
    const Vec<double, 2>& touch_motion) const {
  return (touch_motion * display_size_).ConvertTo<int64_t>();
}

void Editor::EveryFrame() {
  graphics_->SetDrawColor(kDarkGray)->Clear();
  map_->Draw(graphics_, window_in_world_);
  DrawMapGrid();
  DrawSelectionHighlight();
  DrawCursorHighlight();

  // sidebar_.Draw();

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
  // if (sidebar_.Contains({event.x, event.y})) {
  //  sidebar_.OnMouseButtonDown(event);
  //} else {
  SetCursorGridPosition({event.x, event.y});
  map_->SetTileIndex(last_cursor_map_position_, 1, 4);
  //}
}

void Editor::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {
  // if (sidebar_.Contains({event.x, event.y})) {
  //  sidebar_.OnMouseButtonUp(event);
  //}
}

void Editor::OnMouseMotion(const SDL_MouseMotionEvent& event) {
  // if (sidebar_.Contains({event.x, event.y})) {
  //  sidebar_.OnMouseMotion(event);
  //}

  SetCursorGridPosition({event.x, event.y});
}

void Editor::OnMouseWheel(const SDL_MouseWheelEvent& event) {
  // TODO try to pass to UI; if it isn't on any UI, pass to map
}

void Editor::OnFingerDown(const SDL_TouchFingerEvent& event) {
  two_finger_touch_.OnFingerDown(event);
}
void Editor::OnFingerUp(const SDL_TouchFingerEvent& event) {
  two_finger_touch_.OnFingerUp(event);
}
void Editor::OnFingerMotion(const SDL_TouchFingerEvent& event) {
  two_finger_touch_.OnFingerMotion(event);

  /*
  int num_fingers = SDL_GetNumTouchFingers(event.touchId);
  if (num_fingers != 2)
    return;

  Point<double, 2> this_touch{event.x + event.dx, event.y + event.dy};
  int this_touch_index = -1;

  Point<double, 2> fingers[2];

  for (int i = 0; i < 2; ++i) {
    SDL_Finger* finger = SDL_GetTouchFinger(event.touchId, i);
    if (!finger)
      return;

    fingers[i] = {finger->x, finger->y};

    if (fingers[i] == this_touch)
      this_touch_index = i;

    Print("diff: ", this_touch - fingers[i]);
  }
  if (this_touch_index == -1) {
    std::cerr << "no match??\n";
    return;
  } else {
    std::cerr << "match\n";
  }

  int other_index = (this_touch_index + 1) % 2;

  Point<double, 2> rel{event.dx, event.dy};
  Point<double, 2> prev = this_touch - rel;
  double zoom = Distance(prev, fingers[other_index]) /
                Distance(this_touch, fingers[other_index]);

  Point<double, 2> move = rel / 2.;
  std::cerr << "zoom: " << zoom << ", move: " << move.x() << " " << move.y()
            << '\n';
  window_in_world_.pos -= TouchMotionToPixels(rel / 2.);
  */
}

void Editor::DrawMapGrid() {
  Vec<int64_t, 2> grid_size_pixels_ = grid_size_tiles_ * tile_size_;
  Vec<int64_t, 2> phase = window_in_world_.pos % grid_size_pixels_;
  graphics_->SetDrawColor(kGreen);

  // Draw vertical lines
  for (int64_t x = window_in_world_.x() - phase.x();
       x < window_in_world_.x() + window_in_world_.w();
       x += grid_size_pixels_.x()) {
    if (x == 0)
      graphics_->SetDrawColor(kRed);

    world_graphics_.DrawLine({x, window_in_world_.y()},
                             {x, window_in_world_.y() + window_in_world_.h()});
    if (x == 0)
      graphics_->SetDrawColor(kGreen);
  }

  // Draw horizontal lines
  for (int64_t y = window_in_world_.y() - phase.y();
       y < window_in_world_.y() + window_in_world_.h();
       y += grid_size_pixels_.y()) {
    if (y == 0)
      graphics_->SetDrawColor(kRed);

    world_graphics_.DrawLine({window_in_world_.x(), y},
                             {window_in_world_.x() + window_in_world_.w(), y});
    if (y == 0)
      graphics_->SetDrawColor(kGreen);
  }
}

void Editor::DrawCursorHighlight() {
  graphics_->SetDrawColor(kGreen);
  world_graphics_.DrawRect(
      {last_cursor_map_position_ * tile_size_, tile_size_});
}

void Editor::DrawSelectionHighlight() {
  graphics_->SetDrawColor(kGreen);
  world_graphics_.DrawRect(map_selection_ * tile_size_);
}

void Editor::SetCursorGridPosition(const Point<>& screen_pos) {
  Point<> map_pos = (screen_pos + window_in_world_.pos) / tile_size_;
  last_cursor_map_position_ = {map_pos.x(), map_pos.y()};
}

Editor::TwoFingerHandler::TwoFingerHandler(Editor* editor) : editor_(editor) {}

void Editor::TwoFingerHandler::OnPinch(const Point<double, 2>& center,
                                       double pinch_factor) {
  std::cerr << "pinch amount: " << pinch_factor << '\n';
}

void Editor::TwoFingerHandler::OnDrag(const Vec<double, 2>& drag_amount) {
  editor_->window_in_world_.pos -= editor_->TouchMotionToPixels(drag_amount);
}

}  // namespace tilemapeditor