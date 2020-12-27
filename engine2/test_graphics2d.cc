#include "engine2/test_graphics2d.h"

namespace engine2 {
namespace test {

Graphics2D* TestGraphics2D::SetScale(float x_scale, float y_scale) {
  return this;
}

Graphics2D* TestGraphics2D::SetLogicalSize(int width, int height) {
  return this;
}

Rect<int, 2> TestGraphics2D::GetLogicalSize() {
  return {0, 0, 0, 0};
}

Rect<int, 2> TestGraphics2D::GetSize() {
  return {0, 0, 0, 0};
}

Graphics2D* TestGraphics2D::DrawPoint(const Point<int, 2>& p) {
  return this;
}

Graphics2D* TestGraphics2D::DrawLine(const Point<int, 2>& p0,
                                     const Point<int, 2>& p1) {
  return this;
}

Graphics2D* TestGraphics2D::DrawRect(const Rect<int, 2>& rect) {
  return this;
}

Graphics2D* TestGraphics2D::FillRect(const Rect<int, 2>& rect) {
  return this;
}

Graphics2D* TestGraphics2D::DrawTexture(const Texture& texture) {
  return this;
}

Graphics2D* TestGraphics2D::DrawTexture(const Texture& texture,
                                        const Rect<int, 2>& dest) {
  return this;
}

Graphics2D* TestGraphics2D::DrawTexture(const Texture& texture,
                                        const Rect<int, 2>& src,
                                        const Rect<int, 2>& dest) {
  return this;
}

Graphics2D* TestGraphics2D::Clear() {
  return this;
}

Graphics2D* TestGraphics2D::Present() {
  return this;
}

Graphics2D* TestGraphics2D::SetDrawColor(const RgbaColor& color) {
  return this;
}

RgbaColor TestGraphics2D::GetDrawColor() {
  return {0, 0, 0, 0};
}

SDL_Renderer* TestGraphics2D::renderer() const {
  return nullptr;
}

}  // namespace test
}  // namespace engine2