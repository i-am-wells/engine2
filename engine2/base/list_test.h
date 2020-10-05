#ifndef ENGINE2_BASE_LIST_TEST_H_
#define ENGINE2_BASE_LIST_TEST_H_

#include <vector>

#include "engine2/test/test_group.h"

namespace engine2 {
namespace test {

class ListTest : public TestGroup {
 public:
  void TestEmpty();
  void TestAddToHead();
  void TestAddManyToHead();
  void TestUnlink();
  void TestRelink();
  void TestUnlinkTail();
  void TestUnlinkHead();
  void TestUnlinkAll();
  ListTest();
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_BASE_LIST_TEST_H_