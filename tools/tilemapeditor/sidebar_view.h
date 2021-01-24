#ifndef TOOLS_TILEMAPEDITOR_SIDEBAR_VIEW_H_
#define TOOLS_TILEMAPEDITOR_SIDEBAR_VIEW_H_

#include <vector>

#include "engine2/ui/container_view.h"
#include "engine2/ui/text_view.h"

namespace tilemapeditor {

class SidebarView : public engine2::ui::ContainerView {
 public:
  SidebarView(const engine2::Rect<int, 2>& rect,
              engine2::Graphics2D* graphics,
              engine2::Font* font);

  void Draw() const override;

 private:
  engine2::Graphics2D* graphics_;
  engine2::ui::TextView text_view_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_SIDEBAR_VIEW_H_