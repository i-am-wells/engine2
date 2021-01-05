#include "engine2/logic_context.h"

#include "engine2/impl/logic_context_impl.h"

namespace engine2 {

// static
std::unique_ptr<LogicContext> LogicContext::Create() {
  return std::make_unique<LogicContextImpl>();
}

}  // namespace engine2