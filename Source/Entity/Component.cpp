#include "Shiny/ShinyAssert.h"
#include "Shiny/Entity/Component.h"
#include "Shiny/Entity/Entity.h"

namespace Shiny {

// static
ComponentRegistry& ComponentRegistry::instance() {
   static ComponentRegistry singleton;
   return singleton;
}

void Component::destroy() {
   bool destroyed = owner.destroyComponent(this);
   ASSERT(destroyed, "Component not destroyed by owner, possibly already distroyed?");
}

} // namespace Shiny
