#ifndef ENGINE2_UI_IMAGE_VIEW_H_
#define ENGINE2_UI_IMAGE_VIEW_H_

#include "engine2/graphics2d.h"
#include "engine2/texture.h"
#include "engine2/ui/hierarchy_view.h"

namespace engine2 {
namespace ui {

class ImageView : public HierarchyView {
 public:
  ImageView(Texture* texture,
            Graphics2D* graphics,
            const Rect<>& source_rect,
            double scale = 1.);
  Vec<int, 2> GetSize() const override;
  void Draw() const override;

 protected:
  Vec<int64_t, 2> GetScaledTextureSize() const;

  Texture* texture_;
  Graphics2D* graphics_;
  Rect<> source_rect_;
  double scale_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_IMAGE_VIEW_H_