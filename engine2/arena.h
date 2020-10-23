#ifndef ENGINE2_ARENA_H_
#define ENGINE2_ARENA_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "engine2/base/list.h"
#include "engine2/impl/rect_search_tree.h"
#include "engine2/types.h"

namespace engine2 {

template <typename Active, typename Reactive, int N>
class Arena {
  // TODO static asserts about active and reactive?
 public:
  Arena(Rect<int64_t, N> rect, int tree_depth);

  void AddActive(Active* obj);
  void AddReactive(Reactive* obj);

  // TODO
  // void Remove(Rep* obj);
  // void Clear();

  // Ensures obj is stored in the tree correctly. This should be called after
  // obj's size or position changes.
  void Update(Active* obj);

  // For each active object |a|:
  //   For each object |b| overlapping |a|:
  //     call a.OnOverlap(b)
  //   For each object |b| touching |a|:
  //     call a.OnTouch(b)
  void React(Reactive* reactive);
  void ReactAll();

 private:
  class ReactiveWrapper : public RectSearchTree<Active*, N>::RectObject {
   public:
    ReactiveWrapper(Reactive* reactive = nullptr) : reactive_(reactive) {}
    ~ReactiveWrapper() = default;

    operator Reactive*() { return reactive_; }

    Rect<int64_t, N> GetRect() override { return reactive_->GetRect(); }
    void OnOverlap(Active* active) override { reactive_->OnOverlap(active); }
    void OnTouch(Active* active) override { reactive_->OnTouch(active); }

   private:
    Reactive* reactive_;
  };
  List<ReactiveWrapper> reactives_;
  std::unique_ptr<RectSearchTree<Active*, N>> tree_;
  std::unordered_map<Active*, RectSearchTree<Active*, N>*>
      tree_node_for_active_;
};

template <typename Active, typename Reactive, int N>
Arena<Active, Reactive, N>::Arena(Rect<int64_t, N> rect, int tree_depth)
    : tree_(RectSearchTree<Active*, N>::Create(rect, tree_depth)) {}

template <typename Active, typename Reactive, int N>
void Arena<Active, Reactive, N>::AddActive(Active* obj) {
  tree_node_for_active_[obj] = tree_->Insert(obj);
}

template <typename Active, typename Reactive, int N>
void Arena<Active, Reactive, N>::AddReactive(Reactive* obj) {
  reactives_.AddToHead(obj);
}

template <typename Active, typename Reactive, int N>
void Arena<Active, Reactive, N>::Update(Active* obj) {
  RectSearchTree<Active*>* current_node = tree_node_for_active_[obj];
  // TODO debug log here?
  if (!current_node)
    return;

  Rect<> rect = obj->GetRect().GetOverlap(tree_->GetRect());

  // First search down from the current node.
  RectSearchTree<Active*>* proper_home = current_node->Find(rect);

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

template <typename Active, typename Reactive, int N>
void Arena<Active, Reactive, N>::React(Reactive* reactive) {
  tree_->RunCallbacksOn(reactive);
}

template <typename Active, typename Reactive, int N>
void Arena<Active, Reactive, N>::ReactAll() {
  for (auto* list_node = reactives_.Head(); list_node;
       list_node = list_node->Next()) {
    React(list_node->payload);
  }
}

}  // namespace engine2

#endif  // ENGINE2_ARENA_H_