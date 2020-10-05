#include "engine2/test/test_group.h"

#include <iostream>

namespace engine2 {
namespace test {
namespace {

static constexpr char kAnsiEscape = 0x1b;
static constexpr int kAnsiDefault = 0;
static constexpr int kAnsiRed = 31;
static constexpr int kAnsiGreen = 32;

void SetLogColor(int color_code) {
  std::cerr << kAnsiEscape << '[' << color_code << 'm';
}

}  // namespace

TestGroup::Result operator+(const TestGroup::Result& lhs,
                            const TestGroup::Result& rhs) {
  return {lhs.passed + rhs.passed, lhs.failed + rhs.failed};
}

TestGroup::TestGroup(const std::string& name,
                     std::vector<std::function<void()>> tests)
    : name_(name), tests_(tests) {}

void TestGroup::Assert(bool value, const std::string& message, bool abort) {
  if (!value) {
    fail_ = true;
    Log(message);
    if (abort)
      throw std::logic_error(message);
  }
}

TestGroup::Result TestGroup::RunTests() {
  Result result;
  for (auto test : tests_) {
    SetUp();
    try {
      test();
    } catch (std::logic_error err) {
      Log("Aborted test.");
    }
    TearDown();

    if (fail_) {
      ++result.failed;
      fail_ = false;
    }
  }
  result.passed = tests_.size() - result.failed;
  if (result.failed)
    SetLogColor(kAnsiRed);

  Log(name_, ":\t", result.passed, " passed\t", result.failed, " failed");
  SetLogColor(kAnsiDefault);
  return result;
}

}  // namespace test
}  // namespace engine2
