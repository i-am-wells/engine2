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
           const Vec<int, 2>& padding = {},
           const Vec<int, 2>& margin = {});

  void Init();
  void Draw() const override;

  Vec<int, 2> GetSize() const override;

  std::string GetText() const;
  void SetText(const std::string& text);

  Vec<double, 2> GetScale() const { return scale_; }
  void SetScale(const Vec<double, 2>& scale) { scale_ = scale; }

 protected:
  void RenderText();

  Graphics2D* graphics_;
  Font* font_;
  RgbaColor color_;
  std::unique_ptr<Texture> texture_;
  Vec<double, 2> scale_{1, 1};

  std::string text_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_TEXT_VIEW_H_