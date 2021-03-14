#ifndef TOOLS_TILEMAPEDITOR_ACTION_STACK_H_
#define TOOLS_TILEMAPEDITOR_ACTION_STACK_H_

#include <list>
#include <vector>

#include "engine2/tile_map.h"

namespace tilemapeditor {

class ActionStack {
 public:
  struct SetTileIndexData {
    engine2::TileMap::GridPoint point;
    int layer;
    uint16_t tile_index;
    uint16_t prev_tile_index;

    bool operator==(const SetTileIndexData& other) const;
    bool operator!=(const SetTileIndexData& other) const;
  };

  struct Action {
    enum class Type {
      kSetTileIndex,
    };
    Type type;

    union {
      std::vector<SetTileIndexData> set_tile_index_data;
    };

    Action(Type type);
    Action(const Action& other);
    ~Action();
    bool operator==(const Action& other) const;
    bool operator!=(const Action& other) const;
  };

  int Count() const;
  bool Empty() const;
  void Push(Action action);
  void Pop();

  Action& Last();

 private:
  std::list<Action> stack_;
};

}  // namespace tilemapeditor

#endif  // TOOLS_TILEMAPEDITOR_ACTION_STACK_H_