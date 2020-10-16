#ifndef ENGINE2_CALLBACK_WITH_ID_H_
#define ENGINE2_CALLBACK_WITH_ID_H_

#include <cstdint>

#include "engine2/types.h"

namespace engine2 {

struct CallbackWithId {
  Callback callback;
  uint32_t id;
};

}  // namespace engine2

#endif  // ENGINE2_CALLBACK_WITH_ID_H_