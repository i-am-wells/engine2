#include "engine2/basic_window.h"
#include "engine2/impl/basic_graphics2d.h"

namespace engine2 {

BasicWindow::BasicWindow(const Rect& rect,
                         std::unique_ptr<Delegate> delegate,
                         const std::string& name,
                         int sdl_window_flags,
                         int sdl_renderer_flags)
    : name_(name),
      rect_(rect),
      window_flags_(sdl_window_flags),
      renderer_flags_(sdl_renderer_flags),
      delegate_(std::move(delegate)) {}

BasicWindow::~BasicWindow() {
  TearDown();
}

bool BasicWindow::SetUp() {
  window_ = Window::Create(name_, rect_, window_flags_);
  if (!window_)
    return false;

  graphics_ = BasicGraphics2D::Create(*window_, renderer_flags_);
  if (!graphics_)
    return false;

  if (!delegate_ || !delegate_->SetUp(graphics_.get(), window_.get()))
    return false;
  return true;
}

bool BasicWindow::ReadState() {
  return delegate_->ReadState();
}

bool BasicWindow::Draw() {
  if (!delegate_->Draw())
    return false;

  graphics_->Present();
  return true;
}

void BasicWindow::TearDown() {
  if (!window_ || !graphics_)
    return;

  if (delegate_)
    delegate_->TearDown();

  graphics_.reset();
  window_.reset();
}

}  // namespace engine2