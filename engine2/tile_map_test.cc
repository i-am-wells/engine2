#include <sstream>
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

static constexpr Vec<int64_t, 2> kTileSize{16, 16};
static constexpr Vec<int64_t, 2> kGridSize{100, 100};
static constexpr Point<int64_t, 2> kPositionInWorld{-100, -100};

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

TestSprite CreateSprite() {
  return TestSprite(nullptr, /*frame_count=*/0);
}

}  // namespace

void TileMapTest::TestDraw() {
  TileMap map(kTileSize, kGridSize, /*layer_count=*/1, kPositionInWorld,
              /*sprite_cache=*/nullptr);

  TestSprite sprite1 = CreateSprite();
  TestSprite sprite2 = CreateSprite();
  TestSprite sprite3 = CreateSprite();

  map.AddTiles({
      {&sprite1},
      {&sprite2},
      {&sprite3},
  });

  TileMap::GridPoint grid_point{0, 1};
  map.SetTileIndex(grid_point, /*layer=*/0, /*tile_index=*/1);
  ++grid_point.y();
  map.SetTileIndex(grid_point, /*layer=*/0, /*tile_index=*/2);

  Rect<int64_t, 2> window_in_world{-100, -100, 300, 200};
  TestGraphics2D graphics;
  map.Draw(&graphics, window_in_world);

  Point<> stride{0, kTileSize.y()};

  EXPECT_EQ(1, sprite2.draw_calls.size());
  EXPECT_EQ(1, sprite2.CountDraws(stride));
  EXPECT_EQ(1, sprite3.draw_calls.size());
  EXPECT_EQ(1, sprite3.CountDraws(stride * 2l));
}

void TileMapTest::TestSaveAndLoad() {
  SpriteCache sprite_cache(/*texture_cache=*/nullptr);
  sprite_cache.Put("sprite0", Sprite(nullptr));
  sprite_cache.Put("sprite1", Sprite(nullptr));

  Sprite* sprite0 = sprite_cache.Get("sprite0");
  Sprite* sprite1 = sprite_cache.Get("sprite1");

  const Vec<int64_t, 2> kSmallGridSize{10, 10};
  TileMap map(kTileSize, kSmallGridSize, /*layer_count=*/2, kPositionInWorld,
              &sprite_cache, /*empty_initialize=*/true);
  map.AddTiles({
      {nullptr},
      {sprite0},
      {sprite1, Time::Delta::FromSeconds(1)},
  });

  TileMap::GridPoint point{1, 0};
  map.SetTileIndex({1, 0}, /*layer=*/0, /*tile=*/1);
  map.SetTileIndex({2, 0}, /*layer=*/1, /*tile=*/2);

  std::stringstream stream;
  map.Write(stream);

  std::string string_for_debug = stream.str();

  auto map2 = TileMap::Read(stream, &sprite_cache);
  ASSERT_NOT_NULL(map2.get());

  auto new_tile_size = map2->GetTileSize();
  EXPECT_EQ(kTileSize.x(), new_tile_size.x());
  EXPECT_EQ(kTileSize.y(), new_tile_size.y());

  auto new_grid_size = map2->GetGridSize();
  EXPECT_EQ(kSmallGridSize.x(), new_grid_size.x());
  EXPECT_EQ(kSmallGridSize.y(), new_grid_size.y());
  EXPECT_EQ(2, map2->GetLayerCount());

  const Rect<int64_t, 2> kExpectedWorldRect{kPositionInWorld,
                                            kTileSize * kSmallGridSize};
  auto new_world_rect = map2->GetWorldRect();
  EXPECT_EQ(kExpectedWorldRect.x(), new_world_rect.x());
  EXPECT_EQ(kExpectedWorldRect.y(), new_world_rect.y());
  EXPECT_EQ(kExpectedWorldRect.w(), new_world_rect.w());
  EXPECT_EQ(kExpectedWorldRect.h(), new_world_rect.h());

  EXPECT_NULL(map2->GetTileByIndex(0)->sprite);
  EXPECT_EQ(sprite0, map2->GetTileByIndex(1)->sprite);
  EXPECT_EQ(sprite1, map2->GetTileByIndex(2)->sprite);

  // verify all tiles
  int sprite0_count = 0;
  int sprite1_count = 0;
  TileMap::GridPoint p;
  for (p.y() = 0; p.y() < new_grid_size.y(); ++p.y()) {
    for (p.x() = 0; p.x() < new_grid_size.x(); ++p.x()) {
      for (int layer = 0; layer < 2; ++layer) {
        if (p == TileMap::GridPoint{1, 0} && layer == 0) {
          ASSERT_EQ(1, map2->GetTileIndex(p, layer));
          TileMap::Tile* tile0 = map2->GetTile(p, layer);
          ASSERT_NOT_NULL(tile0);
          ASSERT_EQ(sprite0, tile0->sprite);
          EXPECT_EQ(0., tile0->animation_offset.ToSeconds());
          ++sprite0_count;
        } else if (p == TileMap::GridPoint{2, 0} && layer == 1) {
          ASSERT_EQ(2, map2->GetTileIndex(p, layer));
          TileMap::Tile* tile1 = map2->GetTile(p, layer);
          ASSERT_NOT_NULL(tile1);
          ASSERT_EQ(sprite1, tile1->sprite);
          EXPECT_EQ(1., tile1->animation_offset.ToSeconds());
          ++sprite1_count;
        } else {
          ASSERT_EQ(0, map2->GetTileIndex(p, layer));
          TileMap::Tile* tile = map2->GetTile(p, layer);
          ASSERT_NOT_NULL(tile);
          ASSERT_NULL(tile->sprite);
          ASSERT_EQ(0., tile->animation_offset.ToSeconds());
        }
      }
    }
  }
}

TileMapTest::TileMapTest()
    : TestGroup("TileMapTest",
                {
                    std::bind(&TileMapTest::TestDraw, this),
                    std::bind(&TileMapTest::TestSaveAndLoad, this),
                }) {}

}  // namespace test
}  // namespace engine2