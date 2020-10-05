#ifndef ENGINE2_ARENA2D_H_
#define ENGINE2_ARENA2D_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "engine2/base/list.h"
#include "engine2/impl/rect_search_tree.h"
#include "engine2/types.h"

namespace engine2 {

template <typename Active, typename Reactive>
class Arena2D {
  // TODO static asserts about active and reactive?
 public:
  Arena2D(Rect rect, int tree_depth);

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
  class ReactiveWrapper : public RectSearchTree<Active>::RectObject {
   public:
    ReactiveWrapper(Reactive* reactive = nullptr) : reactive_(reactive) {}
    ~ReactiveWrapper() = default;

    operator Reactive*() { return reactive_; }

    Rect GetRect() override { return reactive_->GetRect(); }
    void OnOverlap(Active* active) override { reactive_->OnOverlap(active); }
    void OnTouch(Active* active) override { reactive_->OnTouch(active); }

   private:
    Reactive* reactive_;
  };
  List<ReactiveWrapper> reactives_;
  std::unique_ptr<RectSearchTree<Active>> tree_;
  std::unordered_map<Active*, RectSearchTree<Active>*> tree_node_for_active_;
};

}  // namespace engine2

#include "engine2/arena2d.cc"

#endif  // ENGINE2_ARENA2D_H_