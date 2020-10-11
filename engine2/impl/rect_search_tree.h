#ifndef ENGINE2_IMPL_RECT_SEARCH_TREE_H_
#define ENGINE2_IMPL_RECT_SEARCH_TREE_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "engine2/base/list.h"
#include "engine2/rect.h"

namespace engine2 {

template <typename Rep>
class RectSearchTree {
 public:
  // Create a new tree of depth |tree_depth| spanning |rect|.
  static std::unique_ptr<RectSearchTree> Create(Rect<> rect, int tree_depth);

  // Add an object to the search tree. Returns a pointer to the subtree the
  // object was added to. Note: if you insert the same object twice, its
  // callbacks will run twice per update.
  RectSearchTree* Insert(Rep* obj);

  // Same as Insert(), but search based on the intersection of obj->GetRect()
  // and rect_.
  RectSearchTree* InsertTrimmed(Rep* obj);

  // Store an object in reps_.
  void InsertToSelf(Rep* obj);
  void InsertToSelf(std::unique_ptr<typename List<Rep*>::Node> node);
  // Remove an object from reps_.
  std::unique_ptr<typename List<Rep*>::Node> RemoveFromSelf(Rep* obj);

  class RectObject {
   public:
    virtual Rect<> GetRect() = 0;
    virtual void OnOverlap(Rep* obj) = 0;
    virtual void OnTouch(Rep* obj) = 0;
    virtual ~RectObject() = default;
  };
  // Runs obj's callbacks:
  //  obj->OnOverlap() for every object overlapping obj
  //  obj->OnTouch() for every object touching obj
  template <typename CallbackObject>
  void RunCallbacksOn(CallbackObject* obj);

  RectSearchTree* Find(const Rect<>& rect);

  const Rect<>& GetRect() const { return rect_; }

 private:
  RectSearchTree(Rect<> rect);
  RectSearchTree* InsertForRect(Rep* obj, const Rect<>& rect);
  RectSearchTree* FindInternal(const Rect<>& rect);

  Rect<> rect_;
  std::unique_ptr<RectSearchTree> child_a_;
  std::unique_ptr<RectSearchTree> child_b_;
  List<Rep*> reps_;
};

}  // namespace engine2

// Template implementation
#include "engine2/impl/rect_search_tree.cc"

#endif  // ENGINE2_IMPL_RECT_SEARCH_TREE_H_