#include <iostream>
#include <memory>

#include "engine2/arena2d.h"
#include "engine2/basic_window.h"
#include "engine2/camera2d.h"
#include "engine2/engine2.h"
#include "engine2/graphics2d.h"
#include "engine2/rect_object.h"
#include "engine2/rgba_color.h"
#include "engine2/texture.h"
#include "engine2/window.h"

using engine2::Arena2D;
using engine2::BasicWindow;
using engine2::Camera2D;
using engine2::Graphics2D;
using engine2::Point;
using engine2::Rect;
using engine2::Texture;
using engine2::Window;

class Pirate : public Camera2D::Visible {
 public:
  Pirate(Point position) : position_(position) {}
  ~Pirate() = default;

  void Move(const Point& d) {
    velocity_ += {walk_speed_ * d.x, walk_speed_ * d.y};
  }

  void UpdatePosition() { position_ += velocity_; }

  // Camera2D::Visible implementation
  Rect GetRect() override {
    if (!texture_)
      return {position_, {0, 0}};
    return {position_, texture_->GetSize().size};
  }
  void OnCameraDraw(Camera2D* camera) override {
    camera->InWorldCoords()->DrawTexture(*texture_, GetRect());
  }

  static void LoadTexture(const Graphics2D& graphics) {
    texture_ = Texture::LoadFromImage(graphics, "piratedemo/pirate0.png");
    if (!texture_)
      throw "failed to load texture";
  }

 private:
  int walk_speed_ = 2;
  Point position_;
  Point velocity_{0, 0};
  // TODO implement resource cache!
  static std::unique_ptr<Texture> texture_;
};

std::unique_ptr<Texture> Pirate::texture_;

class DrawDelegate : public BasicWindow::Delegate {
 public:
  DrawDelegate(Arena2D<Camera2D::Visible, Camera2D>* arena, Camera2D* camera)
      : arena_(arena), camera_(camera) {}

  bool SetUp(Graphics2D* graphics, Window* window) override {
    graphics_ = graphics;
    camera_->SetGraphics(graphics);

    graphics_->SetScale(4, 4);

    // Load sprites
    Pirate::LoadTexture(*graphics);
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
  Arena2D<Camera2D::Visible, Camera2D>* arena_;
  Camera2D* camera_;
};

int main(int argc, char** argv) {
  Rect world_rect{0, 0, 1000, 1000};
  Rect screen_rect{0, 0, 800, 600};

  Camera2D camera(world_rect, screen_rect);
  Pirate pirate({10, 10});

  // TODO should arena have a position?
  Arena2D<Camera2D::Visible, Camera2D> arena(world_rect, 1);
  arena.AddActive(&pirate);
  arena.AddReactive(&camera);

  auto context = engine2::Engine2::Create(std::make_unique<BasicWindow>(
      screen_rect, std::make_unique<DrawDelegate>(&arena, &camera),
      "Pirate Demo"));

  context->EveryFrame()->Run([&]() {
    pirate.UpdatePosition();
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

  context->Run();
  return 0;
}