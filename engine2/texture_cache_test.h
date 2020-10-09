#ifndef ENGINE2_TEXTURE_CACHE_TEST_H_
#define ENGINE2_TEXTURE_CACHE_TEST_H_

#include "engine2/graphics2d.h"
#include "engine2/test/test_group.h"
#include "engine2/window.h"

namespace engine2 {
namespace test {

class TextureCacheTest : public TestGroup {
 public:
  void TestGet();
  void TestGetNonexistant();

  TextureCacheTest();
  ~TextureCacheTest();

 private:
  std::unique_ptr<Window> window_;
  std::unique_ptr<Graphics2D> graphics_;
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_TEXTURE_CACHE_TEST_H_