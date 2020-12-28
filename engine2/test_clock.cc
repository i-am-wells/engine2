#include "engine2/test_clock.h"

namespace engine2 {
namespace test {

TestClock::TestClock(Time now_override) : now_override_(now_override) {
  Time::now_override_ = &now_override_;
}

TestClock::~TestClock() {
  Time::now_override_ = nullptr;
}

}  // namespace test
}  // namespace engine2