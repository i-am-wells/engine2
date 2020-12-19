#include <iostream>
#include <memory>
#include <thread>

#include "engine2/camera2d.h"
#include "engine2/graphics2d.h"
#include "engine2/impl/basic_graphics2d.h"
#include "engine2/logic_context.h"
#include "engine2/physics_object.h"
#include "engine2/rgba_color.h"
#include "engine2/space.h"
#include "engine2/state_mutex.h"
#include "engine2/texture.h"
#include "engine2/texture_cache.h"
#include "engine2/timing.h"
#include "engine2/video_context.h"
#include "engine2/window.h"

using engine2::BasicGraphics2D;
using engine2::Camera2D;
using engine2::CollisionOutcome;
using engine2::Graphics2D;
using engine2::PhysicsObject;
using engine2::Point;
using engine2::Rect;
using engine2::Space;
using engine2::StateMutex;
using engine2::Texture;
using engine2::TextureCache;
using engine2::Time;
using engine2::Timing;
using engine2::VideoContext;
using engine2::Window;

static constexpr char kPirateTexturePath[] = "piratedemo/pirate0.png";
static constexpr char kBackgroundTexturePath[] =
    "piratedemo/ship_horizontal.png";
static constexpr char kCannonballTexturePath[] =
    "piratedemo/cannonball_pile.png";

class PhysicsSprite : public Camera2D::Visible {
 public:
  PhysicsSprite(Rect<> rect, double mass_kg) : physics_(rect, mass_kg) {}

  void SetTexture(Texture* texture) { texture_ = texture; }

  // implementation for Camera2D::Visible
  Rect<> GetRect() override { return physics_.GetRect(); }
  void OnCameraDraw(Camera2D* camera) override {
    camera->InWorldCoords()->DrawTexture(*texture_, GetRect());
    // camera->InViewCoords()->SetDrawColor({0, 0, 0, engine2::kOpaque});
    // camera->InWorldCoords()->DrawRect(GetRect());
  }

  // for Space
  PhysicsObject<2>* physics() { return &physics_; }
  CollisionOutcome OnCollideWith(const PhysicsSprite& other) {
    std::cerr << "Here! " << ++count_ << "\n";
    return CollisionOutcome::kStopDead;
  }

 protected:
  PhysicsObject<2> physics_;
  Texture* texture_ = nullptr;
  int count_ = 0;
};

class Cannonball;
class Pirate : public PhysicsSprite {
 public:
  Pirate(Point<> position)
      : PhysicsSprite({position, {16, 32}}, /*mass_kg=*/75) {}

  void Move(const Point<double>& d) { target_velocity += d * 64.; }
  void UpdateVelocity() { physics_.velocity = target_velocity; }

  CollisionOutcome OnCollideWith(const Pirate& other) {
    return CollisionOutcome::kStopDead;
  }
  CollisionOutcome OnCollideWith(const Cannonball& cannonball) {
    return CollisionOutcome::kStopDead;
  }

 private:
  Point<double, 2> target_velocity{};
};

class Cannonball : public PhysicsSprite {
 public:
  Cannonball(Point<> position)
      : PhysicsSprite({position, {10, 10}}, /*mass_kg=*/10) {}

  CollisionOutcome OnCollideWith(const Pirate& other) {
    return CollisionOutcome::kStopDead;
  }
  CollisionOutcome OnCollideWith(const Cannonball& cannonball) {
    return CollisionOutcome::kStopDead;
  }
};

Texture* background = nullptr;

int main(int argc, char** argv) {
  // TODO SDL_Init

  Rect<> world_rect{0, 0, 1000, 1000};
  Point<int, 2> screen_size{200, 150};
  int scale = 8;

  // TODO fix
  Camera2D camera({0, 0, 250, 150}, {0, 0, 250, 150});
  Pirate pirate({10, 100});

  Cannonball cannonball({100, 100});

  Space<2, Pirate, Cannonball> space(world_rect);
  space.Add(&pirate);
  space.Add(&cannonball);

  camera.Follow(&pirate);

  StateMutex state_mutex;

  std::thread video_thread([&] {
    auto video_context = VideoContext::Create();
    // TODO handle exceptions?
    auto window = Window::Create("Pirate Demo",
                                 {Point<int, 2>{0, 0}, screen_size * scale},
                                 /*sdl_window_flags=*/0);
    auto graphics = BasicGraphics2D::Create(*window, /*sdl_renderer_flags=*/0);
    graphics->SetScale(scale);

    TextureCache texture_cache(graphics.get());
    camera.SetGraphics(graphics.get());

    // Load sprites
    pirate.SetTexture(texture_cache.Get(kPirateTexturePath));
    background = texture_cache.Get(kBackgroundTexturePath);
    cannonball.SetTexture(texture_cache.Get(kCannonballTexturePath));

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

    video_context->Run(&state_mutex);
  });

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

  // TODO SDL_Quit

  // Signal video thread to quit
  // TODO maybe this should be built in to LogicContext
  state_mutex.SendSignal(state_mutex.Lock(), StateMutex::Signal::kQuit);
  video_thread.join();
  return 0;
}