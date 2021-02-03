#include "engine2/ui/drop_down_list_view.h"

namespace engine2 {
namespace ui {

DropDownListView::DropDownListView(Graphics2D* graphics,
                                   Font* font,
                                   Handler* handler,
                                   RgbaColor background_color,
                                   RgbaColor text_color,
                                   const Vec<int, 2>& text_padding)
    : ListView(Direction::kVertical),
      graphics_(graphics),
      font_(font),
      handler_(handler),
      background_color_(background_color),
      text_color_(text_color),
      text_padding_(text_padding) {}

void DropDownListView::AddItemToTop(const std::string& item_text) {
  text_views_.emplace_front(this, item_text);
  InsertChild(&(text_views_.front()), 0);
}

void DropDownListView::AddItemToBottom(const std::string& item_text) {
  text_views_.emplace_back(this, item_text);
  AddChild(&(text_views_.back()));
}

void DropDownListView::RemoveItem(int index) {
  if (index < 0 || index >= text_views_.size())
    return;

  text_views_.erase(FindItemByIndex(index));
  RemoveChild(index);
}

void DropDownListView::Clear() {
  text_views_.clear();
  Clear();
}

void DropDownListView::AddItemsToTop(const std::vector<std::string>& items) {
  for (auto riter = items.rbegin(); riter != items.rend(); ++riter) {
    AddItemToTop(*riter);
  }
}

void DropDownListView::AddItemsToBottom(const std::vector<std::string>& items) {
  for (auto& item_text : items)
    AddItemToBottom(item_text);
}

std::string DropDownListView::GetItem(int index) const {
  if (index < 0 || index >= text_views_.size())
    return {};

  return ConstFindItemByIndex(index)->GetText();
}

typename std::list<DropDownListView::ItemTextView>::iterator
DropDownListView::FindItemByIndex(int index) {
  int i = 0;
  for (auto iter = text_views_.begin(); iter != text_views_.end(); ++iter) {
    if (i == index)
      return iter;
    ++i;
  }
  return text_views_.end();
}

std::list<DropDownListView::ItemTextView>::const_iterator
DropDownListView::ConstFindItemByIndex(int index) const {
  int i = 0;
  for (auto iter = text_views_.cbegin(); iter != text_views_.cend(); ++iter) {
    if (i == index)
      return iter;
    ++i;
  }
  return text_views_.end();
}

DropDownListView::ItemTextView::ItemTextView(DropDownListView* list_view,
                                             const std::string& text)
    : TextView(list_view->graphics_,
               list_view->font_,
               text,
               list_view->text_color_,
               list_view->text_padding_),
      list_view_(list_view) {}

void DropDownListView::ItemTextView::Draw() const {
  list_view_->graphics_->SetDrawColor(list_view_->background_color_)
      ->FillRect(GetRect().ConvertTo<int64_t>());

  TextView::Draw();
}

Vec<int, 2> DropDownListView::ItemTextView::GetPadding() const {
  return list_view_->text_padding_;
}

}  // namespace ui
}  // namespace engine2