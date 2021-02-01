#ifndef ENGINE2_UI_CONTAINER_VIEW_H_
#define ENGINE2_UI_CONTAINER_VIEW_H_

#include "engine2/ui/hierarchy_view.h"

namespace engine2 {
namespace ui {

class ContainerView : public HierarchyView {
 public:
  void Draw() const override;

  void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;
  void OnMouseButtonUp(const SDL_MouseButtonEvent& event) override;
  void OnMouseMotion(const SDL_MouseMotionEvent& event) override;

  bool Contains(const engine2::Point<int, 2>& point) const;

 protected:
  HierarchyView* FindChild(const engine2::Point<int, 2>& point);

  std::vector<HierarchyView*> children_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_CONTAINER_VIEW_H_