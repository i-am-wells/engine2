#include "engine2/base/list.h"
#include "engine2/base/list_test.h"
#include "engine2/test/assert_macros.h"

#include <sstream>

namespace engine2 {
namespace test {
namespace {

template <typename T>
std::vector<T> ListToVector(const List<T>& list) {
  std::vector<T> result;
  auto* node = list.Head();
  while (node) {
    result.push_back(node->payload);
    node = node->next.get();
  }
  return result;
}

template <typename T>
std::string VectorToString(const std::vector<T>& vector) {
  std::ostringstream oss;
  oss << '{';
  size_t i = 0;
  for (auto& val : vector) {
    oss << val;
    if (++i != vector.size())
      oss << ", ";
  }
  oss << '}';
  return oss.str();
}

template <typename T>
std::string ListToString(const List<T>& list) {
  return VectorToString(ListToVector(list));
}

}  // namespace

void ListTest::TestEmpty() {
  List<int> list;
  ASSERT_NULL(list.Head());
}

void ListTest::TestAddToHead() {
  List<int> list;
  list.AddToHead(4);
  ASSERT_NOT_NULL(list.Head());
  EXPECT_EQ(4, list.Head()->payload);
}

void ListTest::TestAddManyToHead() {
  List<int> list;
  list.AddToHead(4);
  list.AddToHead(5);
  list.AddToHead(6);

  List<int>::Node* node = list.Head();
  ASSERT_NOT_NULL(node);
  EXPECT_EQ(6, node->payload);

  node = node->next.get();
  ASSERT_NOT_NULL(node);
  EXPECT_EQ(5, node->payload);

  node = node->next.get();
  ASSERT_NOT_NULL(node);
  EXPECT_EQ(4, node->payload);

  EXPECT_NULL(node->next.get());
  EXPECT_EQ("{6, 5, 4}", ListToString(list));
}

void ListTest::TestUnlink() {
  List<int> list;
  list.AddToHead(4);
  list.AddToHead(5);
  list.AddToHead(6);
  auto* node = list.Head();
  ASSERT_NOT_NULL(node);
  ASSERT_NOT_NULL(node->next);
  auto middle = node->next->UnlinkSelf();
  ASSERT_NOT_NULL(middle);
  EXPECT_EQ(5, middle->payload);
  EXPECT_EQ("{6, 4}", ListToString(list));

  EXPECT_NULL(middle->prev);
  EXPECT_NULL(middle->next.get());
}

void ListTest::TestRelink() {
  List<int> list;
  list.AddToHead(4);
  list.AddToHead(5);
  list.AddToHead(6);
  list.AddToHead(list.Head()->next->UnlinkSelf());
  EXPECT_EQ("{5, 6, 4}", ListToString(list));
}

void ListTest::TestUnlinkTail() {
  List<int> list;
  list.AddToHead(4);
  list.AddToHead(5);
  list.AddToHead(6);
  auto tail = list.Head()->next->next->UnlinkSelf();
  EXPECT_EQ("{6, 5}", ListToString(list));
  EXPECT_EQ(4, tail->payload);
  EXPECT_NULL(tail->prev);
  EXPECT_NULL(tail->next.get());
}

void ListTest::TestUnlinkHead() {
  List<int> list;
  list.AddToHead(4);
  list.AddToHead(5);
  list.AddToHead(6);
  auto head = list.Head()->UnlinkSelf();
  EXPECT_EQ("{5, 4}", ListToString(list));
  EXPECT_EQ(6, head->payload);
  EXPECT_NULL(head->prev);
  EXPECT_NULL(head->next.get());
}

void ListTest::TestUnlinkAll() {
  List<int> list;
  list.AddToHead(4);
  auto head = list.Head()->UnlinkSelf();
  EXPECT_NULL(list.Head());
}

ListTest::ListTest()
    : TestGroup("ListTest",
                {
                    std::bind(&ListTest::TestEmpty, this),
                    std::bind(&ListTest::TestAddToHead, this),
                    std::bind(&ListTest::TestAddManyToHead, this),
                    std::bind(&ListTest::TestUnlink, this),
                    std::bind(&ListTest::TestRelink, this),
                    std::bind(&ListTest::TestUnlinkTail, this),
                    std::bind(&ListTest::TestUnlinkHead, this),
                    std::bind(&ListTest::TestUnlinkAll, this),
                }) {}

}  // namespace test
}  // namespace engine2