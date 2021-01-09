#ifndef ENGINE2_UI_LIST_VIEW_H_
#define ENGINE2_UI_LIST_VIEW_H_

#include <vector>

#include "engine2/ui/view.h"

namespace engine2 {
namespace ui {

class ListView : public View {
 public:
  enum class Direction { kHorizontal, kVertical };

  ListView(Direction direction);

  void AddChild(View* child);
  void RemoveChild(int index);

  // View implementation
  virtual void SetPosition(const Point<int, 2>& position);
  virtual Rect<int, 2> GetRect() const;

  virtual void Draw() const;

  virtual void OnMouseEnter(const SDL_MouseMotionEvent& event);
  virtual void OnMouseLeave(const SDL_MouseMotionEvent& event);
  virtual void OnMouseDown(const SDL_MouseButtonEvent& event);
  virtual void OnMouseUp(const SDL_MouseButtonEvent& event);

 private:
  void ChangeSize(const Point<int, 2>& child_size);
  View* FindChild(const Point<int, 2>& point);
  void UpdateChildPositions(int start_index);

  Direction direction_;
  Rect<int, 2> rect_;
  std::vector<View*> children_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_LIST_VIEW_H_