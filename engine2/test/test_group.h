#ifndef ENGINE2_TEST_TEST_GROUP_H_
#define ENGINE2_TEST_TEST_GROUP_H_

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "engine2/base/build_string.h"

namespace engine2 {
namespace test {

class TestGroup {
 public:
  struct Result {
    int passed = 0;
    int failed = 0;
  };

  void Assert(bool value, const std::string& message, bool abort = true);

  template <typename... Ts>
  static void Log(Ts... vals) {
    std::cerr << BuildString(vals...) << std::endl;
  }

  virtual void SetUp() {}
  virtual void TearDown() {}

  TestGroup(const std::string& name, std::vector<std::function<void()>> tests);
  Result RunTests();

 private:
  std::string name_;
  std::vector<std::function<void()>> tests_;
  bool fail_ = false;
};

TestGroup::Result operator+(const TestGroup::Result& lhs,
                            const TestGroup::Result& rhs);

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_TEST_TEST_GROUP_H_