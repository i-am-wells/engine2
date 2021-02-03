#ifndef ENGINE2_UI_TEXT_VIEW_H_
#define ENGINE2_UI_TEXT_VIEW_H_

#include "engine2/font.h"
#include "engine2/ui/hierarchy_view.h"

namespace engine2 {
namespace ui {

class TextView : public HierarchyView {
 public:
  TextView(Graphics2D* graphics,
           Font* font,
           const std::string& text,
           RgbaColor color,
           const Vec<int, 2>& padding = {});

  void Draw() const override;

  std::string GetText() const;

 protected:
  void RenderText(const std::string& text);
  void RenderText(const std::string& text, const Vec<int, 2>& padding);

  Graphics2D* graphics_;
  Font* font_;
  RgbaColor color_;
  std::unique_ptr<Texture> texture_;
  Vec<int, 2> texture_size_{};

  std::string text_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_TEXT_VIEW_H_