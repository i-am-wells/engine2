#include "engine2/offset_graphics2d.h"

namespace engine2 {

OffsetGraphics2D::OffsetGraphics2D(Graphics2D* graphics, Point<int, 2>* offset)
    : graphics_(graphics), offset_(offset) {}

Graphics2D* OffsetGraphics2D::SetScale(float scale) {
  graphics_->SetScale(scale);
  return this;
}

Graphics2D* OffsetGraphics2D::SetScale(float x_scale, float y_scale) {
  graphics_->SetScale(x_scale, y_scale);
  return this;
}

Graphics2D* OffsetGraphics2D::SetLogicalSize(int width, int height) {
  graphics_->SetLogicalSize(width, height);
  return this;
}

Rect<int, 2> OffsetGraphics2D::GetLogicalSize() {
  return graphics_->GetLogicalSize();
}

Rect<int, 2> OffsetGraphics2D::GetSize() {
  return graphics_->GetSize();
}

Graphics2D* OffsetGraphics2D::DrawPoint(const Point<int, 2>& p) {
  graphics_->DrawPoint(OffsetPoint(p));
  return this;
}

Graphics2D* OffsetGraphics2D::DrawLine(const Point<int, 2>& p0,
                                       const Point<int, 2>& p1) {
  graphics_->DrawLine(OffsetPoint(p0), OffsetPoint(p1));
  return this;
}

Graphics2D* OffsetGraphics2D::DrawRect(const Rect<int, 2>& rect) {
  graphics_->DrawRect(OffsetRect(rect));
  return this;
}

Graphics2D* OffsetGraphics2D::FillRect(const Rect<int, 2>& rect) {
  graphics_->FillRect(OffsetRect(rect));
  return this;
}

Graphics2D* OffsetGraphics2D::DrawTexture(const Texture& texture) {
  graphics_->DrawTexture(texture);
  return this;
}

Graphics2D* OffsetGraphics2D::DrawTexture(const Texture& texture,
                                          const Rect<int, 2>& dest) {
  graphics_->DrawTexture(texture, OffsetRect(dest));
  return this;
}

Graphics2D* OffsetGraphics2D::DrawTexture(const Texture& texture,
                                          const Rect<int, 2>& src,
                                          const Rect<int, 2>& dest) {
  graphics_->DrawTexture(texture, src, OffsetRect(dest));
  return this;
}

Graphics2D* OffsetGraphics2D::Clear() {
  graphics_->Clear();
  return this;
}

Graphics2D* OffsetGraphics2D::Present() {
  graphics_->Present();
  return this;
}

Graphics2D* OffsetGraphics2D::SetDrawColor(const RgbaColor& color) {
  graphics_->SetDrawColor(color);
  return this;
}

RgbaColor OffsetGraphics2D::GetDrawColor() {
  return graphics_->GetDrawColor();
}

Point<int, 2> OffsetGraphics2D::OffsetPoint(const Point<int, 2>& point) const {
  return point - *offset_;
}

Rect<int, 2> OffsetGraphics2D::OffsetRect(const Rect<int, 2>& rect) const {
  return {OffsetPoint(rect.pos), rect.size};
}

}  // namespace engine2