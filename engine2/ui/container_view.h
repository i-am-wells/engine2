#ifndef ENGINE2_UI_CONTAINER_VIEW_H_
#define ENGINE2_UI_CONTAINER_VIEW_H_

#include "engine2/ui/view.h"

namespace engine2 {
namespace ui {

class ContainerView : public View {
 public:
  ContainerView(const Rect<int, 2>& rect, std::vector<View*> views);

  void SetPosition(const engine2::Point<int, 2>& position) override;
  engine2::Rect<int, 2> GetRect() const override { return rect_; }

  void Draw() const override;

  void OnMouseButtonDown(const SDL_MouseButtonEvent& event) override;
  void OnMouseButtonUp(const SDL_MouseButtonEvent& event) override;
  void OnMouseMotion(const SDL_MouseMotionEvent& event) override;

  bool Contains(const engine2::Point<int, 2>& point) const;

 protected:
  engine2::ui::View* FindChild(const engine2::Point<int, 2>& point);

  Rect<int, 2> rect_;
  std::vector<engine2::ui::View*> views_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_CONTAINER_VIEW_H_