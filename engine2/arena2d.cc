//#include "engine2/arena2d.h"

#include <cstdint>

namespace engine2 {

template <typename Active, typename Reactive>
Arena2D<Active, Reactive>::Arena2D(Rect rect, int tree_depth)
    : tree_(RectSearchTree<Active>::Create(rect, tree_depth)) {}

template <typename Active, typename Reactive>
void Arena2D<Active, Reactive>::AddActive(Active* obj) {
  tree_node_for_active_[obj] = tree_->Insert(obj);
}

template <typename Active, typename Reactive>
void Arena2D<Active, Reactive>::AddReactive(Reactive* obj) {
  reactives_.AddToHead(obj);
}

template <typename Active, typename Reactive>
void Arena2D<Active, Reactive>::Update(Active* obj) {
  RectSearchTree<Active>* current_node = tree_node_for_active_[obj];
  // TODO debug log here?
  if (!current_node)
    return;

  Rect rect = obj->GetRect().GetOverlap(tree_->GetRect());

  // First search down from the current node.
  RectSearchTree<Active>* proper_home = current_node->Find(rect);

  // If it doesn't belong below current_node, search from root.
  if (!proper_home)
    proper_home = tree_->Find(rect);

  // If it doesn't overlap the tree at all, store in root.
  if (!proper_home)
    proper_home = tree_.get();

  if (proper_home != current_node) {
    proper_home->InsertToSelf(current_node->RemoveFromSelf(obj));
    tree_node_for_active_[obj] = proper_home;
  }
}

template <typename Active, typename Reactive>
void Arena2D<Active, Reactive>::React(Reactive* reactive) {
  tree_->RunCallbacksOn(reactive);
}

template <typename Active, typename Reactive>
void Arena2D<Active, Reactive>::ReactAll() {
  for (auto* list_node = reactives_.Head(); list_node;
       list_node = list_node->Next()) {
    React(list_node->payload);
  }
}

}  // namespace engine2