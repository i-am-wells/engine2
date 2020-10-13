#include <iostream>
#include <memory>

#include "engine2/arena2d.h"
#include "engine2/basic_window.h"
#include "engine2/camera2d.h"
#include "engine2/engine2.h"
#include "engine2/graphics2d.h"
#include "engine2/physics_object.h"
#include "engine2/rgba_color.h"
#include "engine2/texture.h"
#include "engine2/texture_cache.h"
#include "engine2/window.h"

using engine2::Arena2D;
using engine2::BasicWindow;
using engine2::Camera2D;
using engine2::Graphics2D;
using engine2::PhysicsObject;
using engine2::Point;
using engine2::Rect;
using engine2::Texture;
using engine2::TextureCache;
using engine2::Window;

// class Object {
//  public:
//   Object(const std::string& texture_path, const Point<>& position, double
//   mass_kg);
// };

class Pirate {
 public:
  static constexpr char kTexturePath[] = "piratedemo/pirate0.png";

  Pirate(Point<> position)
      : visible_(this), physics_({position, {16, 32}}, /*mass_kg=*/75) {}
  ~Pirate() = default;

  void Move(const Point<double>& d) { physics_.velocity += d * walk_speed_; }

  void PhysicsUpdate() { physics_.Update(); }

  class Visible : public Camera2D::Visible {
   public:
    Rect<> GetRect() override { return pirate_->physics_.GetRect(); }
    void OnCameraDraw(Camera2D* camera) override {
      camera->InWorldCoords()->DrawTexture(*Pirate::texture_, GetRect());
    }
    Visible(Pirate* pirate) : pirate_(pirate) {}
    ~Visible() = default;

   private:
    Pirate* pirate_;
  };

  static void SetTexture(Texture* texture) {
    if (!texture)
      throw "failed to load texture";
    texture_ = texture;
  }

  Visible* GetVisible() { return &visible_; }

 private:
  friend class Visible;
  Visible visible_;
  double walk_speed_ = 64;  // pixels per second
  PhysicsObject physics_;
  static Texture* texture_;
};

Texture* Pirate::texture_ = nullptr;

/*
class Cannonball : public Camera2D::Visible {
 public:
  static constexpr kTexturePath = "piratedemo/cannonball.png";
};
Texture* Cannonball::texture_ = nullptr;
*/
class DrawDelegate : public BasicWindow::Delegate {
 public:
  DrawDelegate(Arena2D<Camera2D::Visible, Camera2D>* arena, Camera2D* camera)
      : texture_cache_(nullptr), arena_(arena), camera_(camera) {}

  bool SetUp(Graphics2D* graphics, Window* window) override {
    graphics_ = graphics;
    camera_->SetGraphics(graphics);
    texture_cache_.SetGraphics(graphics);

    graphics_->SetScale(4, 4);

    // Load sprites
    Pirate::SetTexture(texture_cache_.Get(Pirate::kTexturePath));
    return true;
  }

  bool ReadState() override {
    arena_->React(camera_);
    return true;
  }

  bool Draw() override {
    graphics_->SetDrawColor({255, 255, 255, engine2::kOpaque})->Clear();
    camera_->Draw();
    return true;
  }
  void TearDown() override {}

 private:
  Graphics2D* graphics_;
  TextureCache texture_cache_;
  Arena2D<Camera2D::Visible, Camera2D>* arena_;
  Camera2D* camera_;
};

int main(int argc, char** argv) {
  Rect<> world_rect{0, 0, 1000, 1000};
  Rect<> screen_rect{0, 0, 800, 600};

  Camera2D camera(world_rect, screen_rect);
  Pirate pirate({10, 10});

  // TODO should arena have a position?
  Arena2D<Camera2D::Visible, Camera2D> arena(world_rect, 1);
  arena.AddActive(pirate.GetVisible());
  arena.AddReactive(&camera);

  auto context = engine2::Engine2::Create(std::make_unique<BasicWindow>(
      screen_rect, std::make_unique<DrawDelegate>(&arena, &camera),
      "Pirate Demo"));

  context->EveryFrame()->Run([&]() {
    pirate.PhysicsUpdate();
    arena.Update(pirate.GetVisible());
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

  context->Run();
  return 0;
}