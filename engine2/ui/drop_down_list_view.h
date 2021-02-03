#ifndef ENGINE2_UI_DROP_DOWN_LIST_VIEW_H_
#define ENGINE2_UI_DROP_DOWN_LIST_VIEW_H_

#include <list>

#include "engine2/font.h"
#include "engine2/rgba_color.h"
#include "engine2/ui/list_view.h"
#include "engine2/ui/text_view.h"

namespace engine2 {
namespace ui {

class DropDownListView : public ListView {
 public:
  // Implement this class to handle events coming from the drop down list.
  class Handler {
   public:
    virtual void OnChoose(int index, const std::string& list_item) = 0;
  };

  DropDownListView(Graphics2D* graphics,
                   Font* font,
                   Handler* handler,
                   RgbaColor background_color = {255, 255, 255, kOpaque},
                   RgbaColor text_color = {0, 0, 0, kOpaque},
                   const Vec<int, 2>& text_padding = {2, 2});

  void AddItemToTop(const std::string& item_text);
  void AddItemToBottom(const std::string& item_text);
  void RemoveItem(int index);
  void Clear();

  void AddItemsToTop(const std::vector<std::string>& items);
  void AddItemsToBottom(const std::vector<std::string>& items);

  std::string GetItem(int index) const;

 private:
  class ItemTextView : public TextView {
   public:
    ItemTextView(DropDownListView* list_view, const std::string& text);

    Vec<int, 2> GetPadding() const override;
    void Draw() const override;

   private:
    DropDownListView* list_view_;
  };
  std::list<ItemTextView>::iterator FindItemByIndex(int index);
  std::list<ItemTextView>::const_iterator ConstFindItemByIndex(int index) const;

  Graphics2D* graphics_;
  Font* font_;
  RgbaColor background_color_;
  RgbaColor text_color_;
  Vec<int, 2> text_padding_;

  friend class ItemTextView;
  std::list<ItemTextView> text_views_;

  Handler* handler_;
};

}  // namespace ui
}  // namespace engine2

#endif  // ENGINE2_UI_DROP_DOWN_LIST_VIEW_H_