#ifndef ENGINE2_GRAPHICS2D_H_
#define ENGINE2_GRAPHICS2D_H_

#include <SDL2/SDL_render.h>

#include "engine2/point.h"
#include "engine2/rect.h"
#include "engine2/rgba_color.h"
#include "engine2/texture.h"

namespace engine2 {

// TODO Rect<int, 2>

class Graphics2D {
 public:
  virtual ~Graphics2D() = default;

  // Methods related to canvas size and drawing scale
  virtual Graphics2D* SetScale(float scale) = 0;
  virtual Graphics2D* SetScale(float x_scale, float y_scale) = 0;
  virtual Graphics2D* SetLogicalSize(int width, int height) = 0;
  virtual Rect<int, 2> GetLogicalSize() = 0;
  virtual Rect<int, 2> GetSize() = 0;

  // Drawing methods
  virtual Graphics2D* DrawPoint(const Point<int, 2>& p) = 0;
  virtual Graphics2D* DrawLine(const Point<int, 2>& p0,
                               const Point<int, 2>& p1) = 0;
  virtual Graphics2D* DrawRect(const Rect<int, 2>& rect) = 0;
  virtual Graphics2D* FillRect(const Rect<int, 2>& rect) = 0;

  virtual Graphics2D* DrawTexture(const Texture& texture) = 0;
  virtual Graphics2D* DrawTexture(const Texture& texture,
                                  const Rect<int, 2>& dest) = 0;
  virtual Graphics2D* DrawTexture(const Texture& texture,
                                  const Rect<int, 2>& src,
                                  const Rect<int, 2>& dest) = 0;

  virtual Graphics2D* Clear() = 0;
  virtual Graphics2D* Present() = 0;

  virtual Graphics2D* SetDrawColor(const RgbaColor& color) = 0;
  virtual RgbaColor GetDrawColor() = 0;

  virtual SDL_Renderer* renderer() const = 0;
};

}  // namespace engine2

#endif  // ENGINE2_GRAPHICS2D_H_