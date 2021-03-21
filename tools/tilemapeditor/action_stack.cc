#include "tools/tilemapeditor/action_stack.h"

namespace tilemapeditor {

int ActionStack::Count() const {
  return stack_.size();
}

bool ActionStack::Empty() const {
  return stack_.empty();
}

void ActionStack::Push(Action action) {
  stack_.push_back(action);
}

void ActionStack::Pop() {
  stack_.pop_back();
}

ActionStack::Action& ActionStack::Last() {
  return stack_.back();
}

bool ActionStack::Action::operator==(const Action& other) const {
  if (type != other.type)
    return false;

  switch (type) {
    case Type::kSetTileIndex:
      return set_tile_index_data == other.set_tile_index_data;
  }
}

bool ActionStack::Action::operator!=(const Action& other) const {
  return !(*this == other);
}

ActionStack::Action::Action(Type type) : type(type) {
  switch (type) {
    case Type::kSetTileIndex:
      new (&set_tile_index_data) std::vector<SetTileIndexData>();
      break;
  }
}

ActionStack::Action::Action(const Action& other) {
  type = other.type;
  switch (type) {
    case Type::kSetTileIndex:
      new (&set_tile_index_data) std::vector<SetTileIndexData>();
      set_tile_index_data = other.set_tile_index_data;
      break;
  }
}

ActionStack::Action::~Action() {
  switch (type) {
    case Type::kSetTileIndex:
      set_tile_index_data.~vector();
      break;
  }
}

ActionStack::Action& ActionStack::Action::operator=(const Action& other) {
  return *(new (this) Action(other));
}

bool ActionStack::SetTileIndexData::operator==(
    const SetTileIndexData& other) const {
  return point == other.point && layer == other.layer &&
         tile_index == other.tile_index &&
         prev_tile_index == other.prev_tile_index;
}

bool ActionStack::SetTileIndexData::operator!=(
    const SetTileIndexData& other) const {
  return !(*this == other);
}

}  // namespace tilemapeditor