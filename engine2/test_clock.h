#ifndef ENGINE2_TEST_CLOCK_H_
#define ENGINE2_TEST_CLOCK_H_

#include "engine2/time.h"

namespace engine2 {
namespace test {

class TestClock {
 public:
  TestClock(Time now_override);
  ~TestClock();

 private:
  Time now_override_;
};

}  // namespace test
}  // namespace engine2

#endif  // ENGINE2_TEST_CLOCK_H_