#ifndef ENGINE2_OFFSET_GRAPHICS2D_H_
#define ENGINE2_OFFSET_GRAPHICS2D_H_

#include "engine2/graphics2d.h"

namespace engine2 {

class OffsetGraphics2D : public Graphics2D {
 public:
  OffsetGraphics2D(Graphics2D* graphics, Point<int, 2>* offset);
  ~OffsetGraphics2D() override = default;
  void SetUnderlyingGraphics2D(Graphics2D* graphics) { graphics_ = graphics; }

  Graphics2D* SetScale(float scale) override;
  Graphics2D* SetScale(float x_scale, float y_scale) override;
  Graphics2D* SetLogicalSize(int width, int height) override;
  Rect<int, 2> GetLogicalSize() override;
  Rect<int, 2> GetSize() override;

  Graphics2D* DrawPoint(const Point<int, 2>& p) override;
  Graphics2D* DrawLine(const Point<int, 2>& p0,
                       const Point<int, 2>& p1) override;
  Graphics2D* DrawRect(const Rect<int, 2>& rect) override;
  Graphics2D* FillRect(const Rect<int, 2>& rect) override;

  Graphics2D* DrawTexture(const Texture& texture) override;
  Graphics2D* DrawTexture(const Texture& texture,
                          const Rect<int, 2>& dest) override;
  Graphics2D* DrawTexture(const Texture& texture,
                          const Rect<int, 2>& src,
                          const Rect<int, 2>& dest) override;

  Graphics2D* Clear() override;
  Graphics2D* Present() override;

  Graphics2D* SetDrawColor(const RgbaColor& color) override;
  RgbaColor GetDrawColor() override;

  SDL_Renderer* renderer() const override { return graphics_->renderer(); }

 private:
  Point<int, 2> OffsetPoint(const Point<int, 2>& point) const;
  Rect<int, 2> OffsetRect(const Rect<int, 2>& rect) const;
  Graphics2D* graphics_;
  Point<int, 2>* offset_;
};

}  // namespace engine2

#endif  // ENGINE2_OFFSET_GRAPHICS2D_H_