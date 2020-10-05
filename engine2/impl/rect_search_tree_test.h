#ifndef ENGINE2_IMPL_RECT_SEARCH_TREE_TEST_H_
#define ENGINE2_IMPL_RECT_SEARCH_TREE_TEST_H_

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class RectSearchTreeTest : public TestGroup {
 public:
  void TestCreate();
  void TestSingleNode();
  void TestHeight2();
  void TestFindOutsideBounds();

  RectSearchTreeTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_IMPL_RECT_SEARCH_TREE_TEST_H_