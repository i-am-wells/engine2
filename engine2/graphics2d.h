#ifndef ENGINE2_GRAPHICS2D_H_
#define ENGINE2_GRAPHICS2D_H_

#include <SDL2/SDL_render.h>

#include "engine2/point.h"
#include "engine2/rect.h"
#include "engine2/rgba_color.h"
#include "engine2/texture.h"

namespace engine2 {

class Graphics2D {
 public:
  virtual ~Graphics2D() = default;

  // Methods related to canvas size and drawing scale
  virtual Graphics2D* SetScale(float scale) = 0;
  virtual Graphics2D* SetScale(float x_scale, float y_scale) = 0;
  virtual Graphics2D* SetLogicalSize(int width, int height) = 0;
  virtual Rect<> GetLogicalSize() = 0;
  virtual Rect<> GetSize() = 0;

  // Drawing methods
  virtual Graphics2D* DrawPoint(const Point<>& p) = 0;
  virtual Graphics2D* DrawLine(const Point<>& p0, const Point<>& p1) = 0;
  virtual Graphics2D* DrawRect(const Rect<>& rect) = 0;
  virtual Graphics2D* FillRect(const Rect<>& rect) = 0;

  virtual Graphics2D* DrawTexture(const Texture& texture) = 0;
  virtual Graphics2D* DrawTexture(const Texture& texture,
                                  const Rect<>& dest) = 0;
  virtual Graphics2D* DrawTexture(const Texture& texture,
                                  const Rect<>& src,
                                  const Rect<>& dest) = 0;

  virtual Graphics2D* Clear() = 0;
  virtual Graphics2D* Present() = 0;

  virtual Graphics2D* SetDrawColor(const RgbaColor& color) = 0;
  virtual RgbaColor GetDrawColor() = 0;

  virtual SDL_Renderer* renderer() const = 0;
};

}  // namespace engine2

#endif  // ENGINE2_GRAPHICS2D_H_