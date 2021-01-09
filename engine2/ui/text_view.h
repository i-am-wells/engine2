#ifndef ENGINE2_UI_TEXT_VIEW_H_
#define ENGINE2_UI_TEXT_VIEW_H_

#include "engine2/font.h"
#include "engine2/ui/view.h"

namespace engine2 {
namespace ui {

class TextView : public View {
 public:
  TextView(Graphics2D* graphics,
           Font* font,
           const std::string& text,
           RgbaColor color);

  // View implementation
  virtual void SetPosition(const Point<int, 2>& position);
  virtual Rect<int, 2> GetRect() const;

  virtual void Draw() const;

 protected:
  void RenderText(const std::string& text);

  Graphics2D* graphics_;
  Font* font_;
  RgbaColor color_;
  std::unique_ptr<Texture> texture_;
  Rect<int, 2> rect_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_TEXT_VIEW_H_