#ifndef ENGINE2_OFFSET_GRAPHICS2D_H_
#define ENGINE2_OFFSET_GRAPHICS2D_H_

#include "engine2/graphics2d.h"

namespace engine2 {

class OffsetGraphics2D : public Graphics2D {
 public:
  OffsetGraphics2D(Graphics2D* graphics, Point<>* offset);
  ~OffsetGraphics2D() override = default;
  void SetUnderlyingGraphics2D(Graphics2D* graphics) { graphics_ = graphics; }

  Graphics2D* SetScale(float x_scale, float y_scale) override;
  Graphics2D* SetLogicalSize(int width, int height) override;
  Rect<> GetLogicalSize() override;
  Rect<> GetSize() override;

  Graphics2D* DrawPoint(const Point<>& p) override;
  Graphics2D* DrawLine(const Point<>& p0, const Point<>& p1) override;
  Graphics2D* DrawRect(const Rect<>& rect) override;
  Graphics2D* FillRect(const Rect<>& rect) override;

  Graphics2D* DrawTexture(const Texture& texture) override;
  Graphics2D* DrawTexture(const Texture& texture, const Rect<>& dest) override;
  Graphics2D* DrawTexture(const Texture& texture,
                          const Rect<>& src,
                          const Rect<>& dest) override;

  Graphics2D* Clear() override;
  Graphics2D* Present() override;

  Graphics2D* SetDrawColor(const RgbaColor& color) override;
  RgbaColor GetDrawColor() override;

  SDL_Renderer* renderer() const override { return graphics_->renderer(); }

 private:
  Point<> OffsetPoint(const Point<>& point) const;
  Rect<> OffsetRect(const Rect<>& rect) const;
  Graphics2D* graphics_;
  Point<>* offset_;
};

}  // namespace engine2

#endif  // ENGINE2_OFFSET_GRAPHICS2D_H_