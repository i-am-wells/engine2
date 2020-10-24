#ifndef ENGINE2_IMPL_RECT_SEARCH_TREE_H_
#define ENGINE2_IMPL_RECT_SEARCH_TREE_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "engine2/base/list.h"
#include "engine2/rect.h"

namespace engine2 {

template <int N, class Rep>
class RectSearchTree {
 public:
  using Rect = Rect<int64_t, N>;

  // Create a new tree of depth |tree_depth| spanning |rect|.
  static std::unique_ptr<RectSearchTree> Create(const Rect& rect,
                                                int tree_depth);

  // Add an object to the search tree. Returns a pointer to the subtree the
  // object was added to. Note: if you insert the same object twice, its
  // callbacks will run twice per update.
  RectSearchTree* Insert(const Rect& rect, Rep obj);

  // Same as Insert(), but search based on the intersection of obj->GetRect()
  // and rect_.
  RectSearchTree* InsertTrimmed(const Rect& rect, Rep obj);

  // Store an object in reps_.
  void InsertLocal(Rep obj);
  void InsertLocal(std::unique_ptr<typename List<Rep>::Node> node);
  // Remove an object from reps_.
  std::unique_ptr<typename List<Rep>::Node> RemoveFromSelf(Rep obj);

  class OverlapAndTouchReceiver {
   public:
    virtual Rect GetRect(Rep* obj) = 0;
    virtual void OnOverlap(Rep obj) = 0;
    virtual void OnTouch(Rep obj) = 0;
    virtual ~OverlapAndTouchReceiver() = default;
  };

  void FindOverlapsAndTouches(const Rect& rect,
                              bool overlap,
                              bool touch,
                              OverlapAndTouchReceiver* receiver);

  RectSearchTree* Find(const Rect& rect);

  const Rect& GetRect() const { return rect_; }

 private:
  RectSearchTree(Rect rect);
  RectSearchTree* FindInternal(const Rect& rect);

  Rect rect_;
  std::unique_ptr<RectSearchTree> child_a_;
  std::unique_ptr<RectSearchTree> child_b_;
  List<Rep> reps_;
};

// static
template <int N, class Rep>
std::unique_ptr<RectSearchTree<N, Rep>> RectSearchTree<N, Rep>::Create(
    const Rect& rect,
    int tree_depth) {
  if (tree_depth == 0)
    return nullptr;

  auto tree =
      std::unique_ptr<RectSearchTree<N, Rep>>(new RectSearchTree<N, Rep>(rect));

  // Find index and length of longest dimension of rect
  int longest_dimension = 0;
  int64_t longest_dimension_length = 0;
  for (int i = 0; i < N; ++i) {
    if (rect.size[i] > longest_dimension_length) {
      longest_dimension = i;
      longest_dimension_length = rect.size[i];
    }
  }

  int64_t half_longest_length = longest_dimension_length / 2;

  // Rect is divided in half across its longest dimension
  Vec<int64_t, N> child_size_1 = rect.size;
  child_size_1[longest_dimension] = half_longest_length;

  Vec<int64_t, N> child_size_2 = rect.size;
  child_size_2[longest_dimension] =
      longest_dimension_length - half_longest_length;

  Vec<int64_t, N> child_pos_2 = rect.pos;
  child_pos_2[longest_dimension] += half_longest_length;

  Rect child_rect_1{rect.pos, child_size_1};
  Rect child_rect_2{child_pos_2, child_size_2};

  // Recursively create child trees
  --tree_depth;
  tree->child_a_ = Create(child_rect_1, tree_depth),
  tree->child_b_ = Create(child_rect_2, tree_depth);

  return tree;
}

template <int N, class Rep>
RectSearchTree<N, Rep>* RectSearchTree<N, Rep>::Insert(const Rect& rect,
                                                       Rep obj) {
  auto* subtree = Find(rect);
  if (!subtree)
    subtree = this;
  subtree->InsertLocal(obj);
  return subtree;
}

template <int N, class Rep>
RectSearchTree<N, Rep>* RectSearchTree<N, Rep>::InsertTrimmed(const Rect& rect,
                                                              Rep obj) {
  // Trim rect to fit in the tree.
  return Insert(rect.GetOverlap(rect_), obj);
}

template <int N, class Rep>
void RectSearchTree<N, Rep>::FindOverlapsAndTouches(
    const Rect& rect,
    bool overlap,
    bool touch,
    OverlapAndTouchReceiver* receiver) {
  if (!rect_.Overlaps(rect) && !rect_.Touches(rect))
    return;

  // Run with own objects
  for (auto* node = reps_.Head(); node; node = node->Next()) {
    Rect found_rect = receiver->GetRect(&(node->payload));
    if (overlap && rect.Overlaps(found_rect))
      receiver->OnOverlap(node->payload);

    if (touch && rect.Touches(found_rect))
      receiver->OnTouch(node->payload);
  }

  if (child_a_ && child_b_) {
    child_a_->FindOverlapsAndTouches(rect, overlap, touch, receiver);
    child_b_->FindOverlapsAndTouches(rect, overlap, touch, receiver);
  }
}

template <int N, class Rep>
void RectSearchTree<N, Rep>::InsertLocal(Rep obj) {
  reps_.AddToHead(obj);
}

template <int N, class Rep>
void RectSearchTree<N, Rep>::InsertLocal(
    std::unique_ptr<typename List<Rep>::Node> node) {
  reps_.AddToHead(std::move(node));
}

template <int N, class Rep>
std::unique_ptr<typename List<Rep>::Node>
RectSearchTree<N, Rep>::RemoveFromSelf(Rep obj) {
  for (auto* node = reps_.Head(); node; node = node->Next()) {
    if (node->payload == obj)
      return std::move(node->UnlinkSelf());
  }
  return nullptr;
}

template <int N, class Rep>
RectSearchTree<N, Rep>* RectSearchTree<N, Rep>::Find(const Rect& rect) {
  Rect rect_copy = rect;

  // Add one to each dimension so the rect is stored in the next node up if it
  // is near a boundary. This allows OnTouch() to work across node boundaries.
  for (int i = 0; i < N; ++i)
    ++rect_copy.size[i];

  return FindInternal(rect_copy);
}

template <int N, class Rep>
RectSearchTree<N, Rep>* RectSearchTree<N, Rep>::FindInternal(const Rect& rect) {
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

template <int N, class Rep>
RectSearchTree<N, Rep>::RectSearchTree(Rect rect) : rect_(rect) {}

}  // namespace engine2

#endif  // ENGINE2_IMPL_RECT_SEARCH_TREE_H_