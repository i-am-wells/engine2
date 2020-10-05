#ifndef ENGINE2_BASE_PROVIDER_H_
#define ENGINE2_BASE_PROVIDER_H_

namespace engine2 {

template <typename T>
class Provider {
 public:
  virtual T& operator T() { return Get(); }
  virtual T& Get() = 0;
  virtual ~Provider() = default;
};

}  // namespace engine2

#endif  // ENGINE2_BASE_PROVIDER_H_