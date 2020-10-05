#include "engine2/impl/basic_graphics2d.h"

#include <SDL2/SDL_render.h>

#include "engine2/texture.h"
#include "engine2/window.h"

namespace engine2 {
namespace {

SDL_Rect ToSDLRect(const Rect& rect) {
  return {static_cast<int>(rect.x), static_cast<int>(rect.y),
          static_cast<int>(rect.w), static_cast<int>(rect.h)};
}

}  // namespace

// static
std::unique_ptr<Graphics2D> BasicGraphics2D::Create(const Window& window,
                                                    SDLRendererFlags flags) {
  SDL_Renderer* sdl_renderer =
      SDL_CreateRenderer(window.window_, /*driver_index=*/-1, flags.to_ulong());
  if (!sdl_renderer)
    return nullptr;
  return std::make_unique<BasicGraphics2D>(sdl_renderer);
}

BasicGraphics2D::BasicGraphics2D(SDL_Renderer* sdl_renderer)
    : renderer_(sdl_renderer) {
  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
  SetDrawColor({0, 0, 0, kOpaque});
  Clear();
  Present();
}

BasicGraphics2D::~BasicGraphics2D() {
  if (renderer_)
    SDL_DestroyRenderer(renderer_);
}

Graphics2D* BasicGraphics2D::SetScale(float x_scale, float y_scale) {
  SDL_RenderSetScale(renderer_, x_scale, y_scale);
  return this;
}

Graphics2D* BasicGraphics2D::DrawPoint(const Point& p) {
  SDL_RenderDrawPoint(renderer_, p.x, p.y);
  return this;
}

Graphics2D* BasicGraphics2D::DrawLine(const Point& p0, const Point& p1) {
  SDL_RenderDrawLine(renderer_, p0.x, p0.y, p1.x, p1.y);
  return this;
}

Graphics2D* BasicGraphics2D::DrawRect(const Rect& rect) {
  SDL_Rect sdl_rect = ToSDLRect(rect);
  SDL_RenderDrawRect(renderer_, &sdl_rect);
  return this;
}

Graphics2D* BasicGraphics2D::FillRect(const Rect& rect) {
  SDL_Rect sdl_rect = ToSDLRect(rect);
  SDL_RenderFillRect(renderer_, &sdl_rect);
  return this;
}

Graphics2D* BasicGraphics2D::SetDrawColor(const RgbaColor& color) {
  SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
  return this;
}

RgbaColor BasicGraphics2D::GetDrawColor() {
  RgbaColor color;
  SDL_GetRenderDrawColor(renderer_, &color.r, &color.g, &color.b, &color.a);
  return color;
}

Graphics2D* BasicGraphics2D::Clear() {
  SDL_RenderClear(renderer_);
  return this;
}

Graphics2D* BasicGraphics2D::Present() {
  SDL_RenderPresent(renderer_);
  return this;
}

Graphics2D* BasicGraphics2D::SetLogicalSize(int width, int height) {
  SDL_RenderSetLogicalSize(renderer_, width, height);
  return this;
}

Rect BasicGraphics2D::GetLogicalSize() {
  int w, h;
  SDL_RenderGetLogicalSize(renderer_, &w, &h);
  return {0, 0, w, h};
}

Rect BasicGraphics2D::GetSize() {
  int w, h;
  SDL_GetRendererOutputSize(renderer_, &w, &h);
  return {0, 0, w, h};
}

Graphics2D* BasicGraphics2D::DrawTexture(const Texture& texture) {
  SDL_RenderCopy(renderer_, texture.texture_, nullptr, nullptr);
  return this;
}

Graphics2D* BasicGraphics2D::DrawTexture(const Texture& texture,
                                         const Rect& dest) {
  SDL_Rect sdl_dest = ToSDLRect(dest);
  SDL_RenderCopy(renderer_, texture.texture_, nullptr, &sdl_dest);
  return this;
}

Graphics2D* BasicGraphics2D::DrawTexture(const Texture& texture,
                                         const Rect& src,
                                         const Rect& dest) {
  SDL_Rect sdl_src = ToSDLRect(src);
  SDL_Rect sdl_dest = ToSDLRect(dest);
  SDL_RenderCopy(renderer_, texture.texture_, &sdl_src, &sdl_dest);
  return this;
}

}  // namespace engine2