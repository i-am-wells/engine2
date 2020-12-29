#ifndef ENGINE2_TILE_MAP_TEST_H_
#define ENGINE2_TILE_MAP_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class TileMapTest : public TestGroup {
 public:
  void TestDraw();
  TileMapTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_TILE_MAP_TEST_H_