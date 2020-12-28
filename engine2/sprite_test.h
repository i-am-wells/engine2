#ifndef ENGINE2_SPRITE_TEST_H_
#define ENGINE2_SPRITE_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class SpriteTest : public TestGroup {
 public:
  void TestCreateStatic();
  void TestDraw();
  void TestOffsetDraw();
  void TestUpdate();
  SpriteTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_SPRITE_TEST_H_