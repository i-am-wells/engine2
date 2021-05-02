#include "piratedemo/player.h"
#include "engine2/camera2d.h"
#include "engine2/graphics2d.h"
#include "engine2/sprite_cache.h"
#include "piratedemo/thing.h"
#include "piratedemo/types.h"
#include "piratedemo/wall.h"

using namespace engine2;

namespace piratedemo {
namespace {

constexpr Vec<int, 2> kPlayerSize{16, 32};
constexpr Rect<int, 2> kPlayerHitBox{3, 22, 10, 10};
constexpr Vec<int, 2> kPlayerDrawOffset{-kPlayerHitBox.pos.value[0],
                                        -kPlayerHitBox.pos.value[1]};
constexpr Time::Delta kPlayerFrameDuration =
    Time::Delta::FromMicroseconds(128'000);

constexpr double kPlayerMassKg = 75;
const Direction kPlayerInitialDirection = Direction::kSouth;
const Player::Movement kPlayerInitialMovement = Player::Movement::kStand;

Sprite::AnimationFrame MakeFrame(Point<int, 2> point,
                                 Vec<int, 2> size,
                                 Vec<int, 2> draw_offset,
                                 Time::Delta duration) {
  return {/*source_rect=*/{point * size, size}, draw_offset, duration};
}

Sprite::AnimationFrame MakePlayerFrame(Point<int, 2> point) {
  return MakeFrame(point, kPlayerSize, kPlayerDrawOffset, kPlayerFrameDuration);
}

int SpriteIndex(int direction, int movement) {
  return direction * 2 + movement;
}

int SpriteIndex(Direction direction, Player::Movement movement) {
  return SpriteIndex(int(direction), int(movement));
}

}  // namespace

std::unique_ptr<Texture> Player::sTexture{};
std::array<Sprite, 8> Player::sSprites{};

// static
bool Player::Load(Graphics2D* graphics) {
  if (sTexture)
    return true;

  sTexture = Texture::LoadFromImage(*graphics, "piratedemo/player_sprite.png");
  if (!sTexture)
    return false;

  // If this code changes, SpriteIndex() should probably also change.
  for (int x = 0; x < 4; ++x) {
    sSprites[SpriteIndex(x, 0)] = Sprite(
        sTexture.get(), {MakePlayerFrame({x, 1}), MakePlayerFrame({x, 0})});

    // Walking
    std::vector<Sprite::AnimationFrame> walk_frames;
    for (int y = 1; y < 5; ++y)
      walk_frames.push_back(MakePlayerFrame({x, y}));
    sSprites[SpriteIndex(x, 1)] =
        Sprite(sTexture.get(), std::move(walk_frames));
  }
  return true;
}

Player::Player(Game* game,
               const Point<>& start_point,
               Graphics2D* graphics,
               Camera2D<Thing>* camera)
    : Thing(game,
            graphics,
            camera,
            &sSprites[SpriteIndex(kPlayerInitialDirection,
                                  kPlayerInitialMovement)],
            {start_point, kPlayerHitBox.size},
            kPlayerMassKg),
      direction_(kPlayerInitialDirection),
      movement_(kPlayerInitialMovement) {}

void Player::Face(Direction direction) {
  direction_ = direction;
  SetSprite();
}

void Player::SetMovement(Movement movement) {
  movement_ = movement;
  SetSprite();
}

void Player::OnCollideWith(const Wall& wall,
                           const engine2::Vec<double, 2>& initial_velocity,
                           int dimension) {
  // Walls stop the player
  physics_.velocity[dimension] = 0;
}

Vec<double, 2>& Player::velocity() {
  return physics_.velocity;
}

void Player::SetSprite() {
  active_sprite_ = &sSprites[SpriteIndex(direction_, movement_)];
}

}  // namespace piratedemo