#include "engine2/base/list.h"

namespace engine2 {

template <typename T>
std::unique_ptr<typename List<T>::Node> List<T>::Node::UnlinkSelf() {
  auto self = std::move(prev->next);
  if (next) {
    next->prev = prev;
    prev->next = std::move(next);
  }
  prev = nullptr;
  next = nullptr;
  return std::move(self);
}

template <typename T>
typename List<T>::Node* List<T>::Head() {
  return sentinel.next.get();
}

template <typename T>
void List<T>::AddToHead(std::unique_ptr<typename List<T>::Node> node) {
  node->next = std::move(sentinel.next);
  node->prev = &sentinel;
  if (node->next)
    node->next->prev = node.get();
}

}  // namespace engine2