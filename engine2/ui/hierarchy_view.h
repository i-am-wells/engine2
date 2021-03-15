#ifndef ENGINE2_UI_HIERARCHY_VIEW_H_
#define ENGINE2_UI_HIERARCHY_VIEW_H_

#include "engine2/ui/view.h"

namespace engine2 {
namespace ui {

// Basic View that determines its own size and has its position determined by
// its parent.
//
// Child classes should update child absolute positions in UpdateLayout().
class HierarchyView : public View {
 public:
  HierarchyView(const Vec<int, 2>& padding, const Vec<int, 2>& margin);

  void SetParent(HierarchyView* parent) { parent_ = parent; }

  Point<int, 2> GetRelativePosition() const override;
  void SetRelativePosition(const Point<int, 2>& pos) override;

  Point<int, 2> GetAbsolutePosition() const override;
  Point<int, 2> GetAbsoluteInnerPosition() const override;

  Vec<int, 2> GetMargin() const override;
  Vec<int, 2> GetPadding() const override;

  Vec<int, 2> GetSize() const override;
  void SetSize(const Vec<int, 2>& size) override;

  Rect<int, 2> GetRect() const override;

  void OnSizeChanged() override;

 protected:
  HierarchyView* parent_ = nullptr;
  Point<int, 2> relative_pos_{};
  Vec<int, 2> inner_size_{};
  Vec<int, 2> padding_{};
  Vec<int, 2> margin_{};
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_HIERARCHY_VIEW_H_