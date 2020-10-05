#ifndef ENGINE2_BASE_BUILD_STRING_H_
#define ENGINE2_BASE_BUILD_STRING_H_

#include <sstream>
#include <string>

namespace engine2 {
namespace internal {
template <typename T>
std::string BuildStringInternal(std::ostringstream* oss, T arg) {
  *oss << arg;
  return oss->str();
}
template <typename T, typename... Ts>
std::string BuildStringInternal(std::ostringstream* oss, T first, Ts... rest) {
  *oss << first;
  return BuildStringInternal(oss, rest...);
}
}  // namespace internal

template <typename... Ts>
std::string BuildString(Ts... args) {
  std::ostringstream oss;
  return internal::BuildStringInternal(&oss, args...);
}

}  // namespace engine2

#endif  // ENGINE2_BASE_BUILD_STRING_H_