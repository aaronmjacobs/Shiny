#include "Shiny/Entity/Component.h"

namespace Shiny {

// static
ComponentRegistry& ComponentRegistry::instance() {
   static ComponentRegistry singleton;
   return singleton;
}

} // namespace Shiny
