#include "engine2/ui/container_view.h"

namespace engine2 {
namespace ui {

ContainerView::ContainerView(const Vec<int, 2>& padding,
                             const Vec<int, 2>& margin)
    : HierarchyView(padding, margin) {}

void ContainerView::Draw() const {
  for (View* view : children_)
    view->Draw();
}

void ContainerView::OnMouseButtonDown(const SDL_MouseButtonEvent& event) {
  View* child = FindChild({event.x, event.y});
  if (child)
    child->OnMouseButtonDown(event);
}

void ContainerView::OnMouseButtonUp(const SDL_MouseButtonEvent& event) {
  View* child = FindChild({event.x, event.y});
  if (child)
    child->OnMouseButtonUp(event);
}

void ContainerView::OnMouseMotion(const SDL_MouseMotionEvent& event) {
  View* child = FindChild({event.x, event.y});
  if (child)
    child->OnMouseMotion(event);

  View* child2 = FindChild({event.x + event.xrel, event.y + event.yrel});
  if (child2 && child2 != child)
    child2->OnMouseMotion(event);
}

void ContainerView::OnMouseWheel(const SDL_MouseWheelEvent& event) {
  Point<int, 2> mouse;
  SDL_GetMouseState(&mouse.x(), &mouse.y());

  View* child = FindChild(mouse);
  if (child)
    child->OnMouseWheel(event);
}

bool ContainerView::Contains(const Point<int, 2>& point) const {
  return GetRect().Contains(point);
}

HierarchyView* ContainerView::FindChild(const Point<int, 2>& point) {
  for (HierarchyView* view : children_) {
    if (view->GetRect().Contains(point))
      return view;
  }
  return nullptr;
}

}  // namespace ui
}  // namespace engine2