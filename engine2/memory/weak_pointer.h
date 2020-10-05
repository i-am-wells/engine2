#ifndef ENGINE2_MEMORY_WEAK_POINTER_H_
#define ENGINE2_MEMORY_WEAK_POINTER_H_

// TODO remove
#include <iostream>
#include <string>
#include <unordered_set>

namespace engine2 {

extern const std::string kSomeString;

/*\
 * 1. Include WeakPointer<T>::Factory as the last member of T so it will be
 *    destroyed first when T is destroyed.
 * 2. To create weak pointers to T, call factory.GetWeakPointer().
 * 3. When Factory is destroyed, all weak pointers will be set to null.
 * 4. When a weak pointer is destroyed, it will be unregistered from Factory.
\*/
template <typename T>
class WeakPointer {
 public:
  WeakPointer() : WeakPointer(nullptr, nullptr) {}

  // Returns the raw pointer, which may have been set to null if the pointed-to
  // object was destroyed.
  T* get() { return pointer_; }

  operator bool() { return pointer_; }
  bool operator!() const { return pointer_ == nullptr; }
  T* operator->() { return pointer_; }

  class Factory {
   public:
    // Make a new weak pointer.
    void GetWeakPointer(WeakPointer* result) {
      // result is registered by operator=().
      *result = WeakPointer<T>(this, pointer_);
    }

    explicit Factory(T* pointer) : pointer_(pointer) {}
    ~Factory();

    size_t GetCountForTesting() const { return weaks_.size(); }

   private:
    friend class WeakPointer;
    void Register(WeakPointer* weak) { weaks_.insert(weak); }
    void Unregister(WeakPointer* weak) { weaks_.erase(weak); }

    T* pointer_;
    std::unordered_set<WeakPointer*> weaks_;
  };

  WeakPointer(const WeakPointer& other);
  WeakPointer& operator=(const WeakPointer& other);
  ~WeakPointer();

 private:
  WeakPointer(Factory* factory, T* pointer)
      : factory_(factory), pointer_(pointer) {}

  Factory* factory_;
  T* pointer_;
};

template <typename T>
WeakPointer<T>::WeakPointer(const WeakPointer<T>& other)
    : factory_(other.factory_), pointer_(other.pointer_) {
  if (this != &other && factory_)
    factory_->Register(this);
}

template <typename T>
WeakPointer<T>& WeakPointer<T>::operator=(const WeakPointer<T>& other) {
  if (this != &other) {
    if (factory_)
      factory_->Unregister(this);

    factory_ = other.factory_;
    pointer_ = other.pointer_;
    if (factory_)
      factory_->Register(this);
  }
  return *this;
}

template <typename T>
WeakPointer<T>::~WeakPointer() {
  if (factory_)
    factory_->Unregister(this);
}

template <typename T>
WeakPointer<T>::Factory::~Factory() {
  for (auto* weak : weaks_) {
    weak->factory_ = nullptr;
    weak->pointer_ = nullptr;
  }
}

}  // namespace engine2

#endif  // ENGINE2_MEMORY_WEAK_POINTER_H_
