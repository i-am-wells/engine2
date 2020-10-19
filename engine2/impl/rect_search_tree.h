#ifndef ENGINE2_IMPL_RECT_SEARCH_TREE_H_
#define ENGINE2_IMPL_RECT_SEARCH_TREE_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "engine2/base/list.h"
#include "engine2/rect.h"

namespace engine2 {

template <typename Rep, int N = 2>
class RectSearchTree {
 public:
  // Create a new tree of depth |tree_depth| spanning |rect|.
  static std::unique_ptr<RectSearchTree> Create(Rect<int64_t, N> rect,
                                                int tree_depth);

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
    virtual Rect<int64_t, N> GetRect() = 0;
    virtual void OnOverlap(Rep* obj) = 0;
    virtual void OnTouch(Rep* obj) = 0;
    virtual ~RectObject() = default;
  };
  // Runs obj's callbacks:
  //  obj->OnOverlap() for every object overlapping obj
  //  obj->OnTouch() for every object touching obj
  template <typename CallbackObject>
  void RunCallbacksOn(CallbackObject* obj);

  RectSearchTree* Find(const Rect<int64_t, N>& rect);

  const Rect<int64_t, N>& GetRect() const { return rect_; }

 private:
  RectSearchTree(Rect<int64_t, N> rect);
  RectSearchTree* InsertForRect(Rep* obj, const Rect<int64_t, N>& rect);
  RectSearchTree* FindInternal(const Rect<int64_t, N>& rect);

  Rect<int64_t, N> rect_;
  std::unique_ptr<RectSearchTree> child_a_;
  std::unique_ptr<RectSearchTree> child_b_;
  List<Rep*> reps_;
};

// static
template <typename Rep, int N>
std::unique_ptr<RectSearchTree<Rep, N>> RectSearchTree<Rep, N>::Create(
    Rect<int64_t, N> rect,
    int tree_depth) {
  if (tree_depth == 0)
    return nullptr;

  auto tree =
      std::unique_ptr<RectSearchTree<Rep>>(new RectSearchTree<Rep>(rect));

  int longest_dimension = 0;
  int64_t longest_dimension_length = 0;
  for (int i = 0; i < N; ++i) {
    if (rect.size[i] > longest_dimension_length) {
      longest_dimension = i;
      longest_dimension_length = rect.size[i];
    }
  }

  int64_t half_longest_length = longest_dimension_length / 2;

  Vec<int64_t, N> child_size_1 = rect.size;
  child_size_1[longest_dimension] = half_longest_length;

  Vec<int64_t, N> child_size_2 = rect.size;
  child_size_2[longest_dimension] =
      longest_dimension_length - half_longest_length;

  Vec<int64_t, N> child_pos_2 = rect.pos;
  child_pos_2[longest_dimension] += half_longest_length;

  Rect<int64_t, N> child_rect_1{rect.pos, child_size_1};
  Rect<int64_t, N> child_rect_2{child_pos_2, child_size_2};

  --tree_depth;
  tree->child_a_ = Create(child_rect_1, tree_depth),
  tree->child_b_ = Create(child_rect_2, tree_depth);

  return tree;
}

template <typename Rep, int N>
RectSearchTree<Rep, N>* RectSearchTree<Rep, N>::Insert(Rep* obj) {
  return InsertForRect(obj, obj->GetRect());
}

template <typename Rep, int N>
RectSearchTree<Rep, N>* RectSearchTree<Rep, N>::InsertTrimmed(Rep* obj) {
  // Trim rect to fit in the tree.
  return InsertForRect(obj, obj->GetRect().GetOverlap(rect_));
}

template <typename Rep, int N>
RectSearchTree<Rep, N>* RectSearchTree<Rep, N>::InsertForRect(
    Rep* obj,
    const Rect<int64_t, N>& rect) {
  auto* subtree = Find(rect);
  if (!subtree)
    subtree = this;
  subtree->InsertToSelf(obj);
  return subtree;
}

template <typename Rep, int N>
template <typename CallbackObject>
void RectSearchTree<Rep, N>::RunCallbacksOn(CallbackObject* obj) {
  Rect<> sender_rect = obj->GetRect();
  if (!rect_.Overlaps(sender_rect) && !rect_.Touches(sender_rect))
    return;

  // Run with own objects
  for (auto* node = reps_.Head(); node; node = node->Next()) {
    if ((void*)(node->payload) == (void*)(obj))
      continue;

    Rect<> found_rect = node->payload->GetRect();
    if (sender_rect.Overlaps(found_rect))
      obj->OnOverlap(node->payload);

    if (sender_rect.Touches(found_rect))
      obj->OnTouch(node->payload);
  }

  if (child_a_ && child_b_) {
    child_a_->RunCallbacksOn(obj);
    child_b_->RunCallbacksOn(obj);
  }
}

template <typename Rep, int N>
void RectSearchTree<Rep, N>::InsertToSelf(Rep* obj) {
  reps_.AddToHead(obj);
}

template <typename Rep, int N>
void RectSearchTree<Rep, N>::InsertToSelf(
    std::unique_ptr<typename List<Rep*>::Node> node) {
  reps_.AddToHead(std::move(node));
}

template <typename Rep, int N>
std::unique_ptr<typename List<Rep*>::Node>
RectSearchTree<Rep, N>::RemoveFromSelf(Rep* obj) {
  for (auto* node = reps_.Head(); node; node = node->Next()) {
    if (node->payload == obj)
      return std::move(node->UnlinkSelf());
  }
  return nullptr;
}

template <typename Rep, int N>
RectSearchTree<Rep, N>* RectSearchTree<Rep, N>::Find(
    const Rect<int64_t, N>& rect) {
  Rect<int64_t, N> rect_copy = rect;

  // Add one to each dimension so the rect is stored in the next node up if it
  // is near a boundary. This allows OnTouch() to work across node boundaries.
  for (int i = 0; i < N; ++i)
    ++rect_copy.size[i];

  return FindInternal(rect_copy);
}

template <typename Rep, int N>
RectSearchTree<Rep, N>* RectSearchTree<Rep, N>::FindInternal(
    const Rect<int64_t, N>& rect) {
  if (!rect_.Contains(rect))
    return nullptr;

  if (child_a_ && child_b_) {
    RectSearchTree* result = child_a_->FindInternal(rect);
    if (result)
      return result;

    result = child_b_->FindInternal(rect);
    if (result)
      return result;
  }
  return this;
}

template <typename Rep, int N>
RectSearchTree<Rep, N>::RectSearchTree(Rect<int64_t, N> rect) : rect_(rect) {}

}  // namespace engine2

#endif  // ENGINE2_IMPL_RECT_SEARCH_TREE_H_