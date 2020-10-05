#ifndef ENGINE2_BASE_LIST_H_
#define ENGINE2_BASE_LIST_H_

#include <memory>

namespace engine2 {

template <typename T>
class List {
 public:
  struct Node {
   public:
    T payload;
    Node* prev = nullptr;
    std::unique_ptr<Node> next = nullptr;

    Node() = default;
    explicit Node(T payload) : payload(payload) {}

    Node* Next() { return next.get(); }

    std::unique_ptr<Node> UnlinkSelf() {
      auto self = std::move(prev->next);
      if (next) {
        next->prev = prev;
        prev->next = std::move(next);
      }
      prev = nullptr;
      next = nullptr;
      return std::move(self);
    }
  };

  Node* Head() const { return sentinel.next.get(); }

  void AddToHead(T val) { AddToHead(std::make_unique<Node>(val)); }

  void AddToHead(std::unique_ptr<Node> node) {
    node->next = std::move(sentinel.next);
    node->prev = &sentinel;
    if (node->next)
      node->next->prev = node.get();
    sentinel.next = std::move(node);
  }

 private:
  Node sentinel;
};

}  // namespace engine2

#endif  // ENGINE2_BASE_LIST_H_
