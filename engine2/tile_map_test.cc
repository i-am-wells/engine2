#include <unordered_set>

#include "engine2/camera2d.h"
#include "engine2/test/assert_macros.h"
#include "engine2/test/test_group.h"
#include "engine2/test_graphics2d.h"
#include "engine2/tile_map.h"
#include "engine2/tile_map_test.h"

namespace engine2 {
namespace test {
namespace {

static constexpr Vec<int, 2> kTileSize{16, 16};
static constexpr Vec<int64_t, 2> kGridSize{100, 100};
static constexpr Point<int64_t, 2> kPositionInWorld{-100, -100};

class TestTexture : public Texture {
 public:
  int id;
  TestTexture(int id) : Texture(/*sdl_texture=*/nullptr), id(id) {}
};

class TestSprite : public Sprite {
 public:
  std::vector<Point<int64_t, 2>> draw_calls;

  TestSprite(Texture* texture, int frame_count)
      : Sprite(texture, frame_count) {}

  void Draw(Graphics2D* graphics, const Point<int64_t, 2>& dest) override {
    draw_calls.push_back(dest);
  }

  int CountDraws(const Point<int64_t, 2>& point) {
    int count = 0;
    for (auto& drew_point : draw_calls) {
      if (drew_point == point)
        ++count;
    }
    return count;
  }
};

TileMap CreateTileMap() {
  return TileMap(kTileSize, kGridSize, kPositionInWorld);
}

TileMap CreateInitializedTileMap() {
  return TileMap(kTileSize, kGridSize, kPositionInWorld,
                 /*empty_initialize=*/true);
}

TestSprite CreateSprite(Texture* texture) {
  return TestSprite(texture, /*frame_count=*/0);
}

}  // namespace

void TileMapTest::TestDraw() {
  TestGraphics2D graphics;
  Camera2D camera(Rect<int64_t, 2>{-100, -100, 300, 200},
                  Rect<int64_t, 2>{0, 0, 300, 200}, &graphics);
  TileMap map = CreateTileMap();

  TestTexture texture1(1);
  TestSprite sprite1 = CreateSprite(&texture1);
  TestTexture texture2(2);
  TestSprite sprite2 = CreateSprite(&texture2);

  int sprite_1_idx = map.AddSprite(&sprite1);
  ASSERT_EQ(0, sprite_1_idx);
  int sprite_2_idx = map.AddSprite(&sprite2);
  ASSERT_EQ(1, sprite_2_idx);
  map.SetTile({0, 1}, 0);
  map.SetTile({0, 2}, 1);

  map.Draw(&camera);

  Point<> dest_point_2 = kPositionInWorld;
  dest_point_2.x() += 0 * kTileSize.x();
  dest_point_2.y() += 2 * kTileSize.y();

  EXPECT_EQ(1, sprite2.draw_calls.size());
  EXPECT_EQ(1, sprite2.CountDraws(dest_point_2));
}

TileMapTest::TileMapTest()
    : TestGroup("TileMapTest",
                {
                    std::bind(&TileMapTest::TestDraw, this),
                }) {}

}  // namespace test
}  // namespace engine2