#include "Shiny/ShinyAssert.h"
#include "Shiny/Entity/Entity.h"
#include "Shiny/Scene/Scene.h"

namespace Shiny {

void Entity::destroy() {
   bool destroyed = scene.destroyEntity(this);
   ASSERT(destroyed, "Entity not destroyed by scene, possibly already distroyed?");
}

} // namespace Shiny
