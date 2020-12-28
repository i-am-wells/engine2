#ifndef ENGINE2_IMPL_BASIC_GRAPHICS2D_H_
#define ENGINE2_IMPL_BASIC_GRAPHICS2D_H_

#include <SDL2/SDL_render.h>

#include <bitset>
#include <memory>

#include "engine2/graphics2d.h"
#include "engine2/point.h"
#include "engine2/rect.h"
#include "engine2/rgba_color.h"

namespace engine2 {

class Window;
class Texture;
using SDLRendererFlags = std::bitset<4>;

class BasicGraphics2D : public Graphics2D {
 public:
  // Create a 2D graphics drawing context. Returns a null unique_ptr if
  // something went wrong.
  static std::unique_ptr<Graphics2D> Create(const Window& window,
                                            SDLRendererFlags flags);
  // The destructor destroys sdl_renderer, so don't destroy the renderer
  // yourself if you use this constructor.
  BasicGraphics2D(SDL_Renderer* sdl_renderer);
  ~BasicGraphics2D() override;

  // Most methods return *this so calls can be chained.

  // Methods related to canvas size and drawing scale
  Graphics2D* SetScale(float scale) override;
  Graphics2D* SetScale(float x_scale, float y_scale) override;
  Graphics2D* SetLogicalSize(int width, int height) override;
  Rect<> GetLogicalSize() override;
  Rect<> GetSize() override;

  // Drawing methods
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

  SDL_Renderer* renderer() const override { return renderer_; }

 private:
  friend class Texture;
  SDL_Renderer* renderer_;
};

}  // namespace engine2

#endif  // ENGINE2_IMPL_BASIC_GRAPHICS2D_H_