#ifndef ENGINE2_IMPL_RECT_SEARCH_TREE_H_
#define ENGINE2_IMPL_RECT_SEARCH_TREE_H_

#include <cstdint>
#include <list>
#include <memory>
#include <vector>

#include "engine2/base/list.h"
#include "engine2/rect.h"

namespace engine2 {

// RectSearchTree stores objects so that sets of objects that overlap or touch a
// given rectangle can be retrieved efficiently.
//
// Example:
//  for (SomeObject* object : rect_search_tree.Near(lookup_rect)) {
//    if (lookup_rect.Overlaps(object->GetRect()) {
//      ...
//    } else if (lookup_rect.Touches(object->GetRect()) {
//      ...
//    }
//  }

template <int N, class Rep>
class RectSearchTree {
 public:
  using Rect = Rect<int64_t, N>;
  class Iterator;
  class NearIterator;
  struct NearIterable;

  // Iterators can only be invalidated by their target objects being moved
  // or removed from the tree.
  //
  // Iterator: Visits all objects in the tree.
  Iterator begin() { return Iterator(this); }
  Iterator end() { return Iterator(); }

  // NearIterator: Tries to skip objects that couldn't touch or overlap |rect|.
  // In the best case, visits only |tree_depth| nodes.
  NearIterable Near(Rect rect) { return NearIterable{this, rect}; }
  struct NearIterable {
    RectSearchTree* tree;
    Rect rect;
    NearIterator begin() { return NearIterator(tree, rect); }
    NearIterator end() { return NearIterator(); }
  };

  // Create a new tree of depth |tree_depth| spanning |rect|.
  static std::unique_ptr<RectSearchTree> Create(const Rect& rect,
                                                int tree_depth);

  // Add an object to the search tree. Returns iterator to the subtree the
  // object was added to.
  Iterator Insert(const Rect& rect, Rep obj);

  // Same as Insert(Rect, Rep), but search based on the intersection of rect
  // and rect_.
  Iterator InsertTrimmed(const Rect& rect, Rep obj);

  // Remove an object from the tree (if present).
  void Remove(Iterator&& iterator);

  // Update object's position in the tree and return a new iterator. (The new
  // iterator is rooted at the specific subtree the object was added to.)
  Iterator Move(Iterator&& iterator, Rect dest);

  // Finds the smallest subtree |rect| could belong to.
  RectSearchTree* Find(const Rect& rect);

  const Rect& GetRect() const { return rect_; }

  class Iterator {
   public:
    virtual bool ShouldIncludeSubtree(RectSearchTree* subtree) {
      // All subtrees should be included (but stop when subtree is null).
      return subtree;
    }

    Iterator() = default;
    Iterator(RectSearchTree* start_node);

    RectSearchTree* Subtree() { return node_queue_.front(); }

    Iterator InsertBefore(Rep obj);
    void Erase();

    Rep& operator*() { return *list_iterator_; }
    operator bool() const { return !node_queue_.empty(); }
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const { return !(*this == other); }
    Iterator& operator++() {
      Advance();
      return *this;
    }

   protected:
    void Init(RectSearchTree* start_node) {
      if (ShouldIncludeSubtree(start_node)) {
        list_iterator_ = start_node->reps_.begin();
        node_queue_.push_back(start_node);

        // If start_node is empty, advance until a valid state is reached.
        if (start_node->reps_.empty())
          Advance();
      }
    }

    void Advance();

    std::list<RectSearchTree*> node_queue_;
    typename std::list<Rep>::iterator list_iterator_;
  };

  class NearIterator : public Iterator {
   public:
    NearIterator() = default;
    NearIterator(RectSearchTree* start_node, Rect rect);

    bool ShouldIncludeSubtree(RectSearchTree* subtree) override {
      // Include only subtrees that touch or overlap |rect_|.
      return subtree &&
             (rect_.Overlaps(subtree->rect_) || rect_.Touches(subtree->rect_));
    }

    NearIterator& operator++() {
      Iterator::Advance();
      return *this;
    }

   private:
    Rect rect_;
  };

 private:
  RectSearchTree(Rect rect);
  RectSearchTree* FindInternal(const Rect& rect);
  RectSearchTree* FindOrNull(const Rect& rect);
  Iterator InsertLocal(Rep obj);

  Rect rect_;
  std::unique_ptr<RectSearchTree> child_a_;
  std::unique_ptr<RectSearchTree> child_b_;
  std::list<Rep> reps_;
};  // namespace engine2

template <int N, class Rep>
RectSearchTree<N, Rep>::Iterator::Iterator(RectSearchTree* start_node) {
  Init(start_node);
}
template <int N, class Rep>
RectSearchTree<N, Rep>::NearIterator::NearIterator(RectSearchTree* start_node,
                                                   Rect rect)
    : rect_(rect) {
  Iterator::Init(start_node);
}

template <int N, class Rep>
typename RectSearchTree<N, Rep>::Iterator
RectSearchTree<N, Rep>::Iterator::InsertBefore(Rep obj) {
  Iterator result(Subtree());
  result.list_iterator_ = Subtree()->reps_.insert(list_iterator_, obj);
  return result;
}

template <int N, class Rep>
void RectSearchTree<N, Rep>::Iterator::Erase() {
  Subtree()->reps_.erase(list_iterator_);
}

template <int N, class Rep>
bool RectSearchTree<N, Rep>::Iterator::operator==(const Iterator& other) const {
  if (node_queue_.empty() && other.node_queue_.empty())
    return true;

  if (node_queue_.empty() != other.node_queue_.empty())
    return false;

  return (node_queue_.front() == other.node_queue_.front()) &&
         (list_iterator_ == other.list_iterator_);
}

template <int N, class Rep>
void RectSearchTree<N, Rep>::Iterator::Advance() {
  // We assume that we're in a valid non-end state: node_queue_ is non-empty,
  // current node reps_ is non-empty, and list_iterator_ points somewhere inside
  // reps_ before end.

  bool in_same_node = true;

  // Keep going until we reach a non-empty node or we reach the end.
  while (!node_queue_.empty()) {
    // Stop if we're still inside current_node.
    RectSearchTree* current_node = node_queue_.front();
    if (in_same_node && (list_iterator_ != current_node->reps_.end()) &&
        (++list_iterator_ != current_node->reps_.end())) {
      return;
    }

    if (ShouldIncludeSubtree(current_node->child_a_.get()))
      node_queue_.push_back(current_node->child_a_.get());

    if (ShouldIncludeSubtree(current_node->child_b_.get()))
      node_queue_.push_back(current_node->child_b_.get());

    // Try to get the next subtree and point list_iterator_ at its beginning.
    node_queue_.pop_front();
    in_same_node = false;

    if (node_queue_.empty())
      return;

    if (!node_queue_.front()->reps_.empty()) {
      list_iterator_ = node_queue_.front()->reps_.begin();
      return;
    }

    // Note: loop only if current_node is empty
  }
}

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
typename RectSearchTree<N, Rep>::Iterator RectSearchTree<N, Rep>::Insert(
    const Rect& rect,
    Rep obj) {
  return Find(rect)->InsertLocal(obj);
}

template <int N, class Rep>
typename RectSearchTree<N, Rep>::Iterator RectSearchTree<N, Rep>::InsertLocal(
    Rep obj) {
  reps_.push_front(obj);
  return Iterator{this};
}

template <int N, class Rep>
typename RectSearchTree<N, Rep>::Iterator RectSearchTree<N, Rep>::InsertTrimmed(
    const RectSearchTree<N, Rep>::Rect& rect,
    Rep obj) {
  // Trim rect to fit in the tree.
  return Insert(rect.GetOverlap(rect_), obj);
}

template <int N, class Rep>
void RectSearchTree<N, Rep>::Remove(Iterator&& iterator) {
  iterator.Erase();
}

template <int N, class Rep>
typename RectSearchTree<N, Rep>::Iterator RectSearchTree<N, Rep>::Move(
    Iterator&& iterator,
    Rect dest) {
  // First try searching below the current node.
  RectSearchTree* subtree = iterator.Subtree()->FindOrNull(dest);
  if (subtree == iterator.Subtree())
    return iterator;

  Iterator new_iterator;
  if (subtree) {
    new_iterator = subtree->InsertLocal(*iterator);
  } else {
    // If object isn't at or below its current node, search from the top.
    new_iterator = Insert(dest, *iterator);
  }
  iterator.Erase();
  return new_iterator;
}

template <int N, class Rep>
RectSearchTree<N, Rep>* RectSearchTree<N, Rep>::Find(
    const RectSearchTree<N, Rep>::Rect& rect) {
  RectSearchTree* subtree = FindOrNull(rect);
  if (!subtree)
    return this;
  return subtree;
}

template <int N, class Rep>
RectSearchTree<N, Rep>* RectSearchTree<N, Rep>::FindOrNull(const Rect& rect) {
  // Add one to each dimension so the rect is stored in the next node up if it
  // is near a boundary. This allows OnTouch() to work across node boundaries.
  Rect rect_copy = rect;
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