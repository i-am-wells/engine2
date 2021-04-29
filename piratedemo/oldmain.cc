#include <iostream>
#include <memory>
#include <thread>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "engine2/camera2d.h"
#include "engine2/font.h"
#include "engine2/graphics2d.h"
#include "engine2/impl/basic_graphics2d.h"
#include "engine2/init.h"
#include "engine2/logic_context.h"
#include "engine2/physics_object.h"
#include "engine2/rect_object.h"
#include "engine2/rgba_color.h"
#include "engine2/space.h"
#include "engine2/state_mutex.h"
#include "engine2/texture.h"
#include "engine2/texture_cache.h"
#include "engine2/timing.h"
#include "engine2/ui/list_view.h"
#include "engine2/ui/text_view.h"
#include "engine2/video_context.h"
#include "engine2/window.h"

using namespace engine2;

static constexpr char kPirateTexturePath[] = "piratedemo/pirate0.png";
static constexpr char kBackgroundTexturePath[] =
    "piratedemo/ship_horizontal.png";
static constexpr char kCannonballTexturePath[] =
    "piratedemo/cannonball_pile.png";

class SpriteObject : public Camera2D::Visible, public RectObject<2> {
 public:
  SpriteObject(Rect<double, 2> rect, double mass_kg)
      : RectObject<2>(rect, mass_kg) {}

  void SetTexture(Texture* texture) { texture_ = texture; }

  // implementation for Camera2D::Visible
  void OnCameraDraw(Camera2D* camera) override {
    camera->InWorldCoords()->DrawTexture(*texture_, GetRect());
    // camera->InViewCoords()->SetDrawColor({0, 0, 0, engine2::kOpaque});
    // camera->InWorldCoords()->DrawRect(GetRect());
  }
  Rect<> GetRect() override {
    return RectObject<2>::GetRect().template ConvertTo<int64_t>();
  }

 protected:
  Texture* texture_ = nullptr;
  int count_ = 0;
};

class Cannonball;
class Pirate : public SpriteObject {
 public:
  Pirate(Point<double, 2> position)
      : SpriteObject({position, {16, 32}}, /*mass_kg=*/75.) {}

  void Move(const Point<double>& d) { target_velocity += d * 64.; }
  void UpdateVelocity() { physics_.velocity = target_velocity; }

  // TODO rename: OnTouch()
  void OnCollideWith(const Pirate& other,
                     const Vec<double, 2>& other_velocity,
                     int dimension) {
    // TODO: signal that there should be just one collision
    std::cerr << "pirate -> pirate\n";
    physics_.velocity[dimension] = 0;
  }
  void OnCollideWith(const Cannonball& cannonball,
                     const Vec<double, 2>& other_velocity,
                     int dimension) {
    std::cerr << "pirate -> cannonball\n";
  }

 private:
  Point<double, 2> target_velocity{};
};

class Cannonball : public SpriteObject {
 public:
  Cannonball(Point<double, 2> position)
      : SpriteObject({position, {10, 10}}, /*mass_kg=*/10) {}

  void OnCollideWith(const Pirate& other,
                     const Vec<double, 2>& other_velocity,
                     int dimension) {
    std::cerr << "cannonball -> pirate\n";
  }
  void OnCollideWith(const Cannonball& cannonball,
                     const Vec<double, 2>& other_velocity,
                     int dimension) {
    std::cerr << "cannonball -> cannonball\n";
  }
};

int main(int argc, char** argv) {
  if (!(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0) ||
      !(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG)) {
    std::cerr << "failed to init SDL: " << SDL_GetError() << "\n";
    return 1;
  }

  // Some details about the world
  Rect<> world_rect{0, 0, 1000, 1000};
  Point<int, 2> screen_size{200, 150};
  int scale = 8;

  auto window =
      Window::Create("Pirate Demo", {Point<int, 2>{0, 0}, screen_size * scale},
                     /*sdl_window_flags=*/0);
  auto graphics = BasicGraphics2D::Create(*window, /*sdl_renderer_flags=*/0);
  graphics->SetScale(scale);

  // Objects and camera
  Camera2D camera({0, 0, 250, 150}, {0, 0, 250, 150}, graphics.get());
  Pirate pirate({10, 100});
  Cannonball cannonball({100, 100});

  camera.Follow(&pirate);

  // Create space and add everything
  Space<2, Pirate, Cannonball> space(world_rect);
  space.Add(&pirate);
  space.Add(&cannonball);

  TextureCache texture_cache(graphics.get());

  // Load sprites
  pirate.SetTexture(texture_cache.Get(kPirateTexturePath));
  background = texture_cache.Get(kBackgroundTexturePath);
  cannonball.SetTexture(texture_cache.Get(kCannonballTexturePath));

  auto video_context = VideoContext::Create();
  video_context->EveryFrame()
      ->OnReadState([&] {
        for (auto& variant : space.Near(camera.GetRect())) {
          std::visit(
              [&camera](auto* object) {
                if (camera.GetRect().Overlaps(object->GetRect()))
                  camera.OnOverlap(object);
              },
              variant);
        }
      })
      ->OnDraw([&] {
        graphics->SetDrawColor({255, 255, 255, engine2::kOpaque})->Clear();
        auto bg_size = background->GetSize();
        camera.InWorldCoords()->DrawTexture(*background, bg_size);
        camera.Draw();
        graphics->Present();
      });

  StateMutex state_mutex;
  std::thread control_thread([&] {
    auto context = engine2::LogicContext::Create();

    Time last_update_time = Time::Now();
    int frame_count = 0;
    context->EveryFrame()->Run([&] {
      ++frame_count;
      Time new_time = Time::Now();
      space.AdvanceTime(new_time - last_update_time);
      last_update_time = new_time;

      // Pirate may have run into something and stopped.
      pirate.UpdateVelocity();
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
    state_mutex.SendSignal(state_mutex.Lock(), StateMutex::Signal::kQuit);
  });
  video_context->Run(&state_mutex);

  control_thread.join();
  IMG_Quit();
  SDL_Quit();
  return 0;
}
