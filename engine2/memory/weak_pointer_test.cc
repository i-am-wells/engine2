#include "engine2/memory/weak_pointer_test.h"
#include "engine2/memory/weak_pointer.h"
#include "engine2/test/assert_macros.h"

#include <cassert>
#include <iostream>

namespace engine2 {
namespace test {

class SomeClass {
 public:
  int x, y;
  SomeClass() : x(4), y(2) {}
  WeakPointer<SomeClass>::Factory weak_factory{this};
};

void WeakPointerTest::TestGet() {
  SomeClass obj;
  WeakPointer<SomeClass> weak;
  obj.weak_factory.GetWeakPointer(&weak);
  ASSERT_NOT_NULL(weak.get());
  EXPECT_EQ(weak->x, 4);
  EXPECT_FALSE(!weak);
}

void WeakPointerTest::TestDestroyFactory() {
  WeakPointer<SomeClass> weak;
  {
    SomeClass obj;
    obj.weak_factory.GetWeakPointer(&weak);
    ASSERT_NOT_NULL(weak.get());
  }
  EXPECT_NULL(weak.get());
  EXPECT_TRUE(!weak);
}

void WeakPointerTest::TestCopy() {
  SomeClass obj;
  WeakPointer<SomeClass> weak0, weak1;
  obj.weak_factory.GetWeakPointer(&weak0);
  weak1 = weak0;
  ASSERT_NOT_NULL(weak0.get());
  EXPECT_EQ(4, weak0->x);
  ASSERT_NOT_NULL(weak1.get());
  EXPECT_EQ(4, weak1->x);

  ASSERT_EQ(2, obj.weak_factory.GetCountForTesting());
}

void WeakPointerTest::TestDestroyWeak() {
  SomeClass obj;
  {
    WeakPointer<SomeClass> weak;
    obj.weak_factory.GetWeakPointer(&weak);
    EXPECT_EQ(1, obj.weak_factory.GetCountForTesting());
  }
  EXPECT_EQ(0, obj.weak_factory.GetCountForTesting());
}

WeakPointerTest::WeakPointerTest()
    : TestGroup("WeakPointerTest",
                {
                    std::bind(&WeakPointerTest::TestGet, this),
                    std::bind(&WeakPointerTest::TestDestroyFactory, this),
                    std::bind(&WeakPointerTest::TestCopy, this),
                    std::bind(&WeakPointerTest::TestDestroyWeak, this),
                }) {}

}  // namespace test
}  // namespace engine2
