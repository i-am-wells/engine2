#include <iostream>
#include <memory>
#include <thread>

#include "engine2/arena2d.h"
#include "engine2/camera2d.h"
#include "engine2/graphics2d.h"
#include "engine2/impl/basic_graphics2d.h"
#include "engine2/logic_context.h"
#include "engine2/physics_object.h"
#include "engine2/rgba_color.h"
#include "engine2/state_mutex.h"
#include "engine2/texture.h"
#include "engine2/texture_cache.h"
#include "engine2/video_context.h"
#include "engine2/window.h"

using engine2::Arena2D;
using engine2::BasicGraphics2D;
using engine2::Camera2D;
using engine2::Graphics2D;
using engine2::PhysicsObject;
using engine2::Point;
using engine2::Rect;
using engine2::StateMutex;
using engine2::Texture;
using engine2::TextureCache;
using engine2::VideoContext;
using engine2::Window;

static constexpr char kPirateTexturePath[] = "piratedemo/pirate0.png";
static constexpr char kBackgroundTexturePath[] =
    "piratedemo/ship_horizontal.png";

class PhysicsSprite : public Camera2D::Visible {
 public:
  PhysicsSprite(Rect<> rect, double mass_kg) : physics_(rect, mass_kg) {}

  void SetTexture(Texture* texture) { texture_ = texture; }

  void PhysicsUpdate() { physics_.Update(); }

  // implementation for Camera2D::Visible
  Rect<> GetRect() override { return physics_.GetRect(); }
  void OnCameraDraw(Camera2D* camera) override {
    camera->InWorldCoords()->DrawTexture(*texture_, GetRect());
  }

 protected:
  PhysicsObject physics_;
  Texture* texture_ = nullptr;
};

class Pirate : public PhysicsSprite {
 public:
  Pirate(Point<> position)
      : PhysicsSprite({position, {16, 32}}, /*mass_kg=*/75) {}

  void Move(const Point<double>& d) { physics_.velocity += d * 64.; }
};

Texture* background = nullptr;

int main(int argc, char** argv) {
  // TODO SDL_Init

  Rect<> world_rect{0, 0, 1000, 1000};
  Rect<> screen_rect{0, 0, 800, 600};

  Camera2D camera(world_rect, screen_rect);
  Pirate pirate({10, 10});
  PhysicsSprite background(/*rect=*/{0, 0, 400, 200}, /*mass_kg=*/1);

  // TODO should arena have a position?
  Arena2D<Camera2D::Visible, Camera2D> arena(world_rect, 1);
  arena.AddActive(&pirate);
  arena.AddActive(&background);
  arena.AddReactive(&camera);

  StateMutex state_mutex;

  std::thread video_thread([&] {
    auto video_context = VideoContext::Create();
    // TODO handle exceptions?
    auto window =
        Window::Create("Pirate Demo", screen_rect, /*sdl_window_flags=*/0);
    auto graphics = BasicGraphics2D::Create(*window, /*sdl_renderer_flags=*/0);
    graphics->SetScale(4, 4);

    TextureCache texture_cache(graphics.get());
    camera.SetGraphics(graphics.get());

    // Load sprites
    pirate.SetTexture(texture_cache.Get(kPirateTexturePath));
    background.SetTexture(texture_cache.Get(kBackgroundTexturePath));

    /* clang-format off */
    video_context->EveryFrame()
      ->OnReadState([&] {
        arena.React(&camera);
      })
      ->OnDraw([&] {
        graphics->SetDrawColor({255, 255, 255, engine2::kOpaque})->Clear();
        camera.Draw();
        graphics->Present();
      });
    /* clang-format on */

    video_context->Run(&state_mutex);
  });

  auto context = engine2::LogicContext::Create();

  context->EveryFrame()->Run([&] {
    pirate.PhysicsUpdate();
    arena.Update(&pirate);
  });

  context->EnableKeyRepeat(false);

  /* clang-format off */
  context->OnKey("Escape")
    ->Pressed([&] { context->Stop(); });

  context->OnKey("w")
    ->Pressed([&] { pirate.Move({0, -1}); })
    ->Released([&] { pirate.Move({0, 1}); });
  
  context->OnKey("a")
    ->Pressed([&] { pirate.Move({-1, 0}); })
    ->Released([&] { pirate.Move({1, 0}); });

  context->OnKey("s")
    ->Pressed([&] { pirate.Move({0, 1}); })
    ->Released([&] { pirate.Move({0, -1}); });

  context->OnKey("d")
    ->Pressed([&] { pirate.Move({1, 0}); })
    ->Released([&] { pirate.Move({-1, 0}); });
  /* clang-format on */

  context->Run(&state_mutex);

  // TODO SDL_Quit
  return 0;
}