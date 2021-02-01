#ifndef ENGINE2_UI_LIST_VIEW_H_
#define ENGINE2_UI_LIST_VIEW_H_

#include <vector>

#include "engine2/ui/container_view.h"

namespace engine2 {
namespace ui {

class ListView : public ContainerView {
 public:
  enum class Direction { kHorizontal, kVertical };

  // Need padding passed in to set initial size, and lookup of GetPadding() on a
  // child class won't work in a constructor.
  ListView(Direction direction, const Vec<int, 2>& padding = {});

  void AddChildren(const std::vector<HierarchyView*>& children);
  void AddChild(HierarchyView* child);
  void RemoveChild(int index);

 private:
  void SetInitialSize(const Vec<int, 2>& padding);
  void Relayout();
  void AddChildInternal(HierarchyView* child);

  Direction direction_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_LIST_VIEW_H_