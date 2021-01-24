#ifndef ENGINE2_UI_LIST_VIEW_H_
#define ENGINE2_UI_LIST_VIEW_H_

#include <vector>

#include "engine2/ui/container_view.h"

namespace engine2 {
namespace ui {

class ListView : public ContainerView {
 public:
  enum class Direction { kHorizontal, kVertical };

  ListView(Direction direction);

  void AddChild(View* child);
  void RemoveChild(int index);

  // View implementation
  void SetPosition(const Point<int, 2>& position) override;
  Rect<int, 2> GetRect() const override;

 private:
  void ChangeSize(const Point<int, 2>& child_size);
  View* FindChild(const Point<int, 2>& point);
  void UpdateChildPositions(int start_index);

  Direction direction_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_LIST_VIEW_H_