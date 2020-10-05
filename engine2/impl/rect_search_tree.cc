//#include "engine2/impl/rect_search_tree.h"

namespace engine2 {

// static
template <typename Rep>
std::unique_ptr<RectSearchTree<Rep>> RectSearchTree<Rep>::Create(
    Rect rect,
    int tree_depth) {
  if (tree_depth == 0)
    return nullptr;

  auto tree =
      std::unique_ptr<RectSearchTree<Rep>>(new RectSearchTree<Rep>(rect));
  Rect child_rect_1, child_rect_2;
  if (rect.w < rect.h) {
    // Horizontal split:
    // +----+----+
    // |    |    |
    // +----+----+
    child_rect_1 = {rect.x, rect.y, rect.w / 2, rect.h};
    child_rect_2 = {rect.x + child_rect_1.w, rect.y, rect.w - child_rect_1.w,
                    rect.h};
  } else {
    // Vertical split:
    // +----+
    // |    |
    // +----+
    // |    |
    // +----+
    child_rect_1 = {rect.x, rect.y, rect.w, rect.h / 2};
    child_rect_2 = {rect.x, rect.y + child_rect_1.h, rect.w,
                    rect.h - child_rect_1.h};
  }

  --tree_depth;
  tree->child_a_ = Create(std::move(child_rect_1), tree_depth),
  tree->child_b_ = Create(std::move(child_rect_2), tree_depth);

  return tree;
}

template <typename Rep>
RectSearchTree<Rep>* RectSearchTree<Rep>::Insert(Rep* obj) {
  return InsertForRect(obj, obj->GetRect());
}

template <typename Rep>
RectSearchTree<Rep>* RectSearchTree<Rep>::InsertTrimmed(Rep* obj) {
  // Trim rect to fit in the tree.
  return InsertForRect(obj, obj->GetRect().GetOverlap(rect_));
}

template <typename Rep>
RectSearchTree<Rep>* RectSearchTree<Rep>::InsertForRect(Rep* obj,
                                                        const Rect& rect) {
  auto* subtree = Find(rect);
  if (!subtree)
    subtree = this;
  subtree->InsertToSelf(obj);
  return subtree;
}

template <typename Rep>
template <typename CallbackObject>
void RectSearchTree<Rep>::RunCallbacksOn(CallbackObject* obj) {
  Rect sender_rect = obj->GetRect();
  if (!rect_.Overlaps(sender_rect) && !rect_.Touches(sender_rect))
    return;

  // Run with own objects
  for (auto* node = reps_.Head(); node; node = node->Next()) {
    if ((void*)(node->payload) == (void*)(obj))
      continue;

    Rect found_rect = node->payload->GetRect();
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

template <typename Rep>
void RectSearchTree<Rep>::InsertToSelf(Rep* obj) {
  reps_.AddToHead(obj);
}

template <typename Rep>
void RectSearchTree<Rep>::InsertToSelf(
    std::unique_ptr<typename List<Rep*>::Node> node) {
  reps_.AddToHead(std::move(node));
}

template <typename Rep>
std::unique_ptr<typename List<Rep*>::Node> RectSearchTree<Rep>::RemoveFromSelf(
    Rep* obj) {
  for (auto* node = reps_.Head(); node; node = node->Next()) {
    if (node->payload == obj)
      return std::move(node->UnlinkSelf());
  }
  return nullptr;
}

template <typename Rep>
RectSearchTree<Rep>* RectSearchTree<Rep>::Find(const Rect& rect) {
  Rect rect_copy = rect;

  // Add one to each dimension so the rect is stored in the next node up if it
  // is near a boundary. This allows OnTouch() to work across node boundaries.
  if (rect.w < rect_.w)
    ++rect_copy.w;

  if (rect.h < rect_.h)
    ++rect_copy.h;

  return FindInternal(rect_copy);
}

template <typename Rep>
RectSearchTree<Rep>* RectSearchTree<Rep>::FindInternal(const Rect& rect) {
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

template <typename Rep>
RectSearchTree<Rep>::RectSearchTree(Rect rect) : rect_(std::move(rect)) {}

}  // namespace engine2
