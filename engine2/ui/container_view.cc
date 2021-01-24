#include "engine2/ui/container_view.h"

namespace engine2 {
namespace ui {

ContainerView::ContainerView(const Rect<int, 2>& rect, std::vector<View*> views)
    : rect_(rect), views_(std::move(views)) {}

void ContainerView::SetPosition(const Point<int, 2>& position) {
  rect_.pos = position;
}

void ContainerView::Draw() const {
  for (View* view : views_)
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

bool ContainerView::Contains(const Point<int, 2>& point) const {
  return rect_.Contains(point);
}

View* ContainerView::FindChild(const Point<int, 2>& point) {
  for (View* view : views_) {
    if (view->GetRect().Contains(point))
      return view;
  }
  return nullptr;
}

}  // namespace ui
}  // namespace engine2