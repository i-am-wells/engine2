#include <cstdint>

#include "engine2/rect.h"
#include "engine2/vec.h"
#include "luadata/object.h"

namespace engine2 {

class LuaDataUtil {
 public:
  template <class K>
  static Vec<int64_t, 2> GetVec2(const luadata::Object& object,
                                 const K& key,
                                 const Vec<int64_t, 2>& default_val = {}) {
    auto obj = object.GetObject(key);
    if (!obj)
      return default_val;
    return {obj->GetInt("x", default_val.x()),
            obj->GetInt("y", default_val.y())};
  }

  template <class K>
  static Rect<> GetRect2(const luadata::Object& object,
                         const K& key,
                         const Rect<>& default_val = {}) {
    auto obj = object.GetObject(key);
    if (!obj)
      return default_val;
    return {
        obj->GetInt("x", default_val.x()), obj->GetInt("y", default_val.y()),
        obj->GetInt("w", default_val.w()), obj->GetInt("h", default_val.h())};
  }
};

}  // namespace engine2