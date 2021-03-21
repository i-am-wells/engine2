#include <algorithm>
#include <cmath>
#include <deque>
#include <fstream>
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

static constexpr Vec<int64_t, 2> kIconSize{8, 8};
static constexpr double kIconScale = 3.;

template <class T>
void PrintR(const std::string& msg, const T& val) {
  std::cerr << msg << " " << val.x() << " " << val.y() << " " << val.w() << " "
            << val.h() << '\n';
}
template <class T>
void PrintV(const std::string& msg, const T& val) {
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
               SpriteCache* sprite_cache,
               const std::string& file_path,
               const std::string& initial_status_text)
    : FrameLoop(/*event_handler=*/this),
      window_(window),
      graphics_(graphics),
      font_(font),
      world_graphics_(graphics_, &(window_in_world_.pos)),
      map_(map),
      file_path_(file_path),
      sprite_cache_(sprite_cache),
      status_bar_(graphics,
                  font,
                  initial_status_text,
                  kBlack,
                  /*padding=*/{10, 10}),
      tile_picker_(this, sprite_cache),
      two_finger_handler_(this),
      two_finger_touch_(&two_finger_handler_),
      tool_buttons_(this, icons_image) {}

void Editor::Init() {
  window_in_world_.pos = {0, 0};
  window_in_world_.size = graphics_->GetSize().size;

  // Set map scale so it fills the window.
  Rect<> map_rect = map_->GetWorldRect();
  if (map_rect.w() > 0 && map_rect.h() > 0) {
    double map_aspect_ratio = double(map_rect.w()) / map_rect.h();
    double window_aspect_ratio =
        double(window_in_world_.size.x()) / window_in_world_.size.y();

    double scale = 1.;
    if (map_aspect_ratio > window_aspect_ratio) {
      // Scale to match width
      scale = double(window_in_world_.size.x()) / map_rect.w();
    } else {
      // Scale to match height
      scale = double(window_in_world_.size.y()) / map_rect.h();
    }
    map_->SetScale(scale);
    scale_ = {scale, scale};
    window_in_world_.size /= scale_;
  }

  status_bar_.Init();
  status_bar_.SetScale({3, 3});
  // Set status bar position
  SetStatusText(status_bar_.GetText());

  // TODO get correct display (and account for display origin!)
  display_size_ = window_->GetDisplaySize();

  tool_buttons_.Init();
  tool_buttons_.SetRelativePosition({10, int(display_size_.y()) - 800});

  tile_picker_.Init();
}

Point<int64_t, 2> Editor::TouchPointToPixels(
    const Point<double, 2>& touch_point) const {
  return touch_point * display_size_;
}

Vec<int64_t, 2> Editor::TouchMotionToPixels(
    const Vec<double, 2>& touch_motion) const {
  return touch_motion * display_size_;
}

void Editor::EveryFrame() {
  graphics_->SetDrawColor(kDarkGray)->Clear();

  // Draw map rectangle
  Rect<> map_draw_rect = map_->GetWorldRect();
  map_draw_rect.pos -= window_in_world_.pos;
  map_draw_rect *= scale_;
  graphics_->SetDrawColor(kGray)->FillRect(map_draw_rect);

  map_->Draw(graphics_, window_in_world_);
  if (move_buffer_)
    move_buffer_->Draw(graphics_, window_in_world_);

  // DrawMapGrid();
  DrawSelectionHighlight();
  DrawCursorHighlight();

  // sidebar_.Draw();
  tile_picker_.Draw();

  // Status bar and background
  graphics_->SetDrawColor(kGray)->FillRect(status_bar_.GetRect());
  status_bar_.Draw();

  tool_buttons_.Draw();

  graphics_->Present();
  framerate_regulator_.Wait();
}

void Editor::OnKeyDown(const SDL_KeyboardEvent& event) {
  if (event.repeat)
    return;

  bool ctrl = event.keysym.mod & KMOD_CTRL;
  switch (event.keysym.sym) {
    case SDLK_s:
      if (ctrl)
        Save();
      break;

    case SDLK_y:
      if (ctrl)
        Redo();
      break;
    case SDLK_z:
      if (ctrl)
        Undo();
      break;

    // TODO: remove once layer picker is done!
    case SDLK_0:
      layer_ = 0;
      break;
    case SDLK_1:
      layer_ = 1;
      break;
    case SDLK_ESCAPE:
      CancelMove();
      break;
    default:
      break;
  }
}

void Editor::OnKeyUp(const SDL_KeyboardEvent& event) {
  if (event.repeat)
    return;

  switch (event.keysym.sym) {
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
    SetCursorGridPosition(point);
    switch (tool_mode_) {
      case ToolMode::kDraw:
        SetSingleTileIndex(last_cursor_map_position_, layer_,
                           tile_picker_.GetSelectedTileIndex(), &undo_stack_);
        break;
      case ToolMode::kErase:
        SetSingleTileIndex(last_cursor_map_position_, layer_, 0, &undo_stack_);
        break;
      case ToolMode::kFill:
        FloodFill(last_cursor_map_position_, layer_,
                  tile_picker_.GetSelectedTileIndex(), &undo_stack_);
        break;
      case ToolMode::kPaste:
      case ToolMode::kSelect:
        map_selection_p0_ = last_cursor_map_position_;
        map_selection_p1_ = last_cursor_map_position_;
        break;
      case ToolMode::kMove: {
        StartMove(ScreenToWorld(point));
        break;
      }
    }
    mouse_down_ = true;
  }
}

void Editor::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {
  // if (sidebar_.Contains({event.x, event.y})) {
  //  sidebar_.OnMouseButtonUp(event);
  //}
  mouse_down_ = false;

  switch (tool_mode_) {
    case ToolMode::kDraw:
    case ToolMode::kErase:
    case ToolMode::kFill:
    case ToolMode::kPaste:
    case ToolMode::kSelect:
      break;
    case ToolMode::kMove:
      FinishMove(ScreenToWorld({event.x, event.y}));
      break;
  }
}

void Editor::OnMouseMotion(const SDL_MouseMotionEvent& event) {
  // if (sidebar_.Contains({event.x, event.y})) {
  //  sidebar_.OnMouseMotion(event);
  //}
  Point<> point{event.x, event.y};
  if (tile_picker_.Contains(point))
    return;

  SetCursorGridPosition(point);
  if (mouse_down_) {
    switch (tool_mode_) {
      case ToolMode::kDraw:
        SetSingleTileIndex(last_cursor_map_position_, layer_,
                           tile_picker_.GetSelectedTileIndex(), &undo_stack_,
                           /*new_stroke=*/false);
        break;
      case ToolMode::kErase:
        SetSingleTileIndex(last_cursor_map_position_, layer_, 0, &undo_stack_,
                           /*new_stroke=*/false);
        break;
      case ToolMode::kFill:
      case ToolMode::kPaste:
      case ToolMode::kSelect:
        map_selection_p1_ = last_cursor_map_position_;
        break;
      case ToolMode::kMove:
        ContinueMove(ScreenToWorld(point));
        break;
    }
  }
}

void Editor::OnMouseWheel(const SDL_MouseWheelEvent& event) {
  // TODO try to pass to UI; if it isn't on any UI, pass to map
}

void Editor::OnFingerDown(const SDL_TouchFingerEvent& event) {
  Point<> point = TouchPointToPixels({event.x, event.y});
  if (tile_picker_.Contains(point)) {
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

void Editor::OnQuit(const SDL_QuitEvent& event) {
  Stop();
}

void Editor::DrawMapGrid() {
  Vec<int64_t, 2> grid_size_pixels_ = grid_size_tiles_ * map_->GetTileSize();
  Vec<int64_t, 2> phase = window_in_world_.pos % grid_size_pixels_;
  graphics_->SetDrawColor(kGreen);

  grid_size_pixels_ *= scale_;
  phase *= scale_;

  // Draw vertical lines
  for (int64_t x = window_in_world_.x() - phase.x();
       x < window_in_world_.x() + window_in_world_.w();
       x += grid_size_pixels_.x()) {
    if (x == 0)
      graphics_->SetDrawColor(kRed);

    world_graphics_.DrawLine(
        {x, window_in_world_.y()},
        {x, int64_t(window_in_world_.y() + window_in_world_.h() * scale_.y())});
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
        {int64_t(window_in_world_.x() + window_in_world_.w() * scale_.x()), y});
    if (y == 0)
      graphics_->SetDrawColor(kGreen);
  }
}

void Editor::DrawCursorHighlight() {
  graphics_->SetDrawColor(kGreen);
  graphics_->DrawRect(
      {WorldToScreen(map_->GridToWorld(last_cursor_map_position_)),
       map_->GetTileSize() * scale_});
}

void Editor::DrawSelectionHighlight() {
  graphics_->SetDrawColor(kGreen);
  Rect<> map_selection = GetMapSelection();
  TileMap::GridPoint map_selection_pos{map_selection.x(), map_selection.y()};
  graphics_->DrawRect({WorldToScreen(map_->GridToWorld(map_selection_pos)),
                       map_selection.size * map_->GetTileSize() * scale_});
}

void Editor::SetCursorGridPosition(const Point<>& screen_pos) {
  last_cursor_map_position_ = map_->WorldToGrid(ScreenToWorld(screen_pos));
}

Rect<int64_t, 2> Editor::GetMapSelection() const {
  Vec<int64_t, 2> diff = map_selection_p1_ - map_selection_p0_;
  return {std::min(map_selection_p0_.x(), map_selection_p1_.x()),
          std::min(map_selection_p0_.y(), map_selection_p1_.y()),
          std::abs(diff.x()) + 1, std::abs(diff.y()) + 1};
}

void Editor::SetMapSelection(const Rect<>& rect) {
  map_selection_p0_ = rect.pos;
  map_selection_p1_ = rect.pos + rect.size - 1l;
}

Point<> Editor::ScreenToWorld(const Point<>& pixel_point) const {
  return (pixel_point / scale_) + window_in_world_.pos;
}

Point<> Editor::WorldToScreen(const Point<>& world_point) const {
  return (world_point - window_in_world_.pos) * scale_;
}

Vec<int64_t, 2> Editor::GetGraphicsLogicalSize() const {
  return graphics_->GetSize().size;
}

void Editor::Redo() {
  UndoRedoInternal(&redo_stack_, &undo_stack_, "Redid ");
}

void Editor::Undo() {
  UndoRedoInternal(&undo_stack_, &redo_stack_, "Undid ");
}

void Editor::Save() {
  std::ofstream stream(file_path_, std::ios::binary);
  if (stream.fail()) {
    Error("Failed to open file " + file_path_ + " for writing.");
    return;
  }

  if (!map_->Write(stream)) {
    Error("Failed to write map file.");
    return;
  }
}

void Editor::SetSingleTileIndex(const engine2::TileMap::GridPoint& point,
                                int layer,
                                uint16_t index,
                                ActionStack* action_stack,
                                bool new_stroke) {
  if (action_stack) {
    if (action_stack->Empty() || new_stroke) {
      action_stack->Push(
          ActionStack::Action(ActionStack::Action::Type::kSetTileIndex));
    }
  }
  SetSingleTileIndexInternal(point, layer, index, action_stack);
}

void Editor::SetSingleTileIndexInternal(
    const engine2::TileMap::GridPoint& point,
    int layer,
    uint16_t index,
    ActionStack* action_stack) {
  if (action_stack && !action_stack->Empty()) {
    ActionStack::SetTileIndexData new_set_tile_data{
        .point = point,
        .layer = layer,
        .tile_index = index,
        .prev_tile_index = map_->GetTileIndex(point, layer),
    };

    std::vector<ActionStack::SetTileIndexData>& data =
        action_stack->Last().set_tile_index_data;
    if (data.empty() || data.back() != new_set_tile_data)
      data.push_back(new_set_tile_data);
  }
  map_->SetTileIndex(point, layer, index);
}

void Editor::FloodFill(const engine2::TileMap::GridPoint& point,
                       int layer,
                       uint16_t index,
                       ActionStack* action_stack) {
  Vec<int64_t, 2> grid_size = map_->GetGridSize();
  uint16_t to_replace = map_->GetTileIndex(point, layer);
  if (to_replace == index)
    return;

  if (action_stack) {
    action_stack->Push(
        ActionStack::Action(ActionStack::Action::Type::kSetTileIndex));
  }

  std::deque<TileMap::GridPoint> points_;
  points_.push_back(point);
  while (!points_.empty()) {
    TileMap::GridPoint fill_point = points_.front();
    if (fill_point.x() >= 0 && fill_point.y() >= 0 &&
        fill_point.x() < grid_size.x() && fill_point.y() < grid_size.y() &&
        map_->GetTileIndex(fill_point, layer) == to_replace) {
      points_.push_back(TileMap::GridPoint{fill_point.x() + 1, fill_point.y()});
      points_.push_back(TileMap::GridPoint{fill_point.x() - 1, fill_point.y()});
      points_.push_back(TileMap::GridPoint{fill_point.x(), fill_point.y() + 1});
      points_.push_back(TileMap::GridPoint{fill_point.x(), fill_point.y() - 1});
      SetSingleTileIndexInternal(fill_point, layer, index, action_stack);
    }

    points_.pop_front();
  }
}

void Editor::StartMove(const engine2::Point<> world_point) {
  if (!map_->GetWorldRect().Contains(world_point))
    return;

  TileMap::GridPoint map_point = map_->WorldToGrid(world_point);

  move_map_original_rect_ = GetMapSelection();
  if (!move_map_original_rect_.Contains(map_point)) {
    move_map_original_rect_ = {map_point.x(), map_point.y(), 1, 1};
  }

  Point<> world_move_corner = map_->GridToWorld(
      {move_map_original_rect_.x(), move_map_original_rect_.y()});
  move_cursor_offset_ = world_point - world_move_corner;
  move_cursor_offset_tiles_ = move_cursor_offset_ / map_->GetTileSize();

  // Cut selection into move buffer
  move_buffer_ = std::make_unique<TileMap>(map_->GetTileSize(),
                                           move_map_original_rect_.size, 1,
                                           world_move_corner, sprite_cache_);
  move_buffer_->SetScale(map_->GetScale());

  // Add all tiles for drawing
  for (int y = 0; y < move_map_original_rect_.h(); ++y) {
    for (int x = 0; x < move_map_original_rect_.w(); ++x) {
      TileMap::GridPoint map_point{move_map_original_rect_.x() + x,
                                   move_map_original_rect_.y() + y};
      uint16_t index = map_->GetTileIndex(map_point, layer_);
      move_buffer_->SetTile(index, *map_->GetTileByIndex(index));
    }
  }

  CopyTiles(map_, move_map_original_rect_.pos, layer_, move_buffer_.get(), {},
            0, move_map_original_rect_.size, nullptr);
  SetTilesInRect(move_map_original_rect_, 0, nullptr);
}

void Editor::ContinueMove(const engine2::Point<> world_point) {
  if (!move_buffer_)
    return;

  Rect<> move_buffer_rect = move_buffer_->GetWorldRect();
  SetMapSelection({map_->WorldToGrid(world_point) - move_cursor_offset_tiles_,
                   move_buffer_rect.size / map_->GetTileSize()});

  // Snap to the nearest tile.
  move_buffer_->SetWorldRect(
      {world_point - move_cursor_offset_, move_buffer_rect.size});
}

void Editor::CancelMove() {
  if (!move_buffer_)
    return;

  // TODO test
  CopyTiles(move_buffer_.get(), {}, 0, map_, move_map_original_rect_.pos,
            layer_, move_buffer_->GetGridSize(), nullptr);
  move_buffer_.reset();
}

void Editor::FinishMove(const engine2::Point<> world_point) {
  if (!move_buffer_)
    return;

  undo_stack_.Push(
      ActionStack::Action(ActionStack::Action::Type::kSetTileIndex));

  Point<> dest_pos = map_->WorldToGrid(world_point) - move_cursor_offset_tiles_;

  // First clear the original rect again.
  // TODO fix undo
  SetTilesInRect(move_map_original_rect_, 0, &undo_stack_);
  CopyTiles(move_buffer_.get(), {}, 0, map_, dest_pos, layer_,
            move_buffer_->GetGridSize(), &undo_stack_);
  move_buffer_.reset();
}

void Editor::SetTilesInRect(const Rect<>& rect,
                            uint16_t index,
                            ActionStack* action_stack) {
  TileMap::GridPoint p;
  for (p.y() = 0; p.y() < rect.h(); ++p.y()) {
    for (p.x() = 0; p.x() < rect.w(); ++p.x()) {
      TileMap::GridPoint dst_point{rect.x() + p.x(), rect.y() + p.y()};
      SetSingleTileIndexInternal(dst_point, layer_, index, action_stack);
    }
  }
}

void Editor::CopyTiles(TileMap* source,
                       const Point<>& source_pos,
                       int source_layer,
                       TileMap* dest,
                       const Point<>& dest_pos,
                       int dest_layer,
                       const Vec<int64_t, 2>& size,
                       ActionStack* action_stack) {
  TileMap::GridPoint p;
  for (p.y() = 0; p.y() < size.y(); ++p.y()) {
    for (p.x() = 0; p.x() < size.x(); ++p.x()) {
      TileMap::GridPoint src_point{source_pos.x() + p.x(),
                                   source_pos.y() + p.y()};
      TileMap::GridPoint dst_point{dest_pos.x() + p.x(), dest_pos.y() + p.y()};

      uint16_t index = source->GetTileIndex(src_point, source_layer);
      // Don't overwrite non-empty tiles with the empty tile.
      if (index == 0)
        continue;

      if (dest == map_) {
        SetSingleTileIndexInternal(dst_point, dest_layer, index, action_stack);
      } else {
        dest->SetTileIndex(dst_point, dest_layer, index);
      }
    }
  }
}

void Editor::UndoRedoInternal(ActionStack* stack,
                              ActionStack* anti_stack,
                              const std::string& undid_or_redid) {
  if (stack->Empty())
    return;

  std::string action_name;
  ActionStack::Action& last_action = stack->Last();
  switch (last_action.type) {
    case ActionStack::Action::Type::kSetTileIndex: {
      bool is_first = true;
      for (auto iter = last_action.set_tile_index_data.rbegin();
           iter != last_action.set_tile_index_data.rend(); ++iter) {
        SetSingleTileIndex(iter->point, iter->layer, iter->prev_tile_index,
                           anti_stack, /*new_stroke=*/is_first);
        is_first = false;
      }
      action_name = "set tiles";
      break;
    }
  }
  SetStatusText(undid_or_redid + action_name);
  stack->Pop();
}

void Editor::SetStatusText(const std::string& status) {
  status_bar_.SetText(status);
  Vec<int, 2> status_bar_size = status_bar_.GetSize();
  status_bar_.SetRelativePosition(graphics_->GetSize().size - status_bar_size);
}

void Editor::Error(const std::string& message) {
  std::cerr << message << '\n';
  SetStatusText(message);
}

Editor::TwoFingerHandler::TwoFingerHandler(Editor* editor) : editor_(editor) {}

void Editor::TwoFingerHandler::OnPinch(const Point<double, 2>& center,
                                       double pinch_factor) {
  editor_->scale_ *= pinch_factor;
  editor_->map_->SetScale(editor_->scale_.x());

  // TODO: center zoom on center!
  Vec<int64_t, 2> old_size = editor_->window_in_world_.size;
  editor_->window_in_world_.size =
      editor_->GetGraphicsLogicalSize() / editor_->scale_;

  editor_->window_in_world_.pos -=
      ((editor_->window_in_world_.size - old_size) * center);
}

void Editor::TwoFingerHandler::OnDrag(const Vec<double, 2>& drag_amount) {
  editor_->window_in_world_.pos -=
      editor_->TouchMotionToPixels(drag_amount / editor_->scale_);
}

Editor::ToolButton::ToolButton(ToolButtonTray* tray,
                               const Rect<>& source_rect,
                               ToolMode mode,
                               const std::string& name)
    : ListView(ListView::Direction::kHorizontal),
      tray_(tray),
      mode_(mode),
      icon_view_(tray->icons_,
                 tray->editor_->graphics_,
                 source_rect,
                 kIconScale,
                 /*padding=*/{8, 8},
                 /*margin=*/{10, 10}),
      name_view_(tray->editor_->graphics_,
                 tray->editor_->font_,
                 name,
                 kBlack,
                 /*padding=*/{},
                 /*margin=*/{10, 10}) {}

void Editor::ToolButton::Init() {
  name_view_.Init();
  name_view_.SetScale({3, 3});
  AddChildren({&icon_view_, &name_view_});
}

void Editor::ToolButton::Draw() const {
  Graphics2D* graphics = tray_->editor_->graphics_;
  if (selected_)
    graphics->SetDrawColor(kRed);
  else
    graphics->SetDrawColor(kWhite);

  graphics->FillRect(icon_view_.GetRect());
  ListView::Draw();
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
      icons_(icons),
      buttons_{
          {this, IconRect({2, 0}), ToolMode::kSelect, "Select"},
          {this, IconRect({3, 2}), ToolMode::kMove, "Move"},
          {this, IconRect({2, 3}), ToolMode::kPaste, "Paste"},
          {this, IconRect({2, 1}), ToolMode::kFill, "Fill"},
          {this, IconRect({1, 1}), ToolMode::kErase, "Erase"},
          {this, IconRect({1, 0}), ToolMode::kDraw, "Draw"},
      } {}

void Editor::ToolButtonTray::Init() {
  for (auto& button : buttons_) {
    button.Init();
    AddChild(&button);
  }
  Select(&(buttons_[5]));
}

void Editor::ToolButtonTray::Select(ToolButton* button) {
  if (selected_)
    selected_->SetSelected(false);

  selected_ = button;
  editor_->tool_mode_ = button->mode();
  selected_->SetSelected(true);
}

}  // namespace tilemapeditor