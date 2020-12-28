#include <functional>

#include "engine2/base/list_test.h"
#include "engine2/impl/rect_search_tree_test.h"
#include "engine2/memory/weak_pointer_test.h"
#include "engine2/physics_object_test.h"
#include "engine2/rect_test.h"
#include "engine2/space_test.h"
#include "engine2/sprite_test.h"
#include "engine2/texture_cache_test.h"
#include "engine2/time_test.h"
#include "engine2/vec_test.h"

namespace engine2 {
namespace test {

void RunAllTests() {
  std::cerr << "\n";
  /* clang-format off */
  TestGroup::Result result = ListTest().RunTests() +
                             PhysicsObjectTest().RunTests() +
                             RectTest().RunTests() +
                             RectSearchTreeTest().RunTests() +
                             SpaceTest().RunTests() +
                             SpriteTest().RunTests() + 
                             TextureCacheTest().RunTests() +
                             TimeTest().RunTests() +
                             VecTest().RunTests() +
                             WeakPointerTest().RunTests();
  /* clang-format on */
  std::cerr << "\nTOTAL: " << result.passed << " passed, " << result.failed
            << " failed\n";
}

}  // namespace test
}  // namespace engine2

int main(int argc, char** argv) {
  engine2::test::RunAllTests();
  return 0;
}