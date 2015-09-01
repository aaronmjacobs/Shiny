#ifndef SHINY_TRANSFORM_H
#define SHINY_TRANSFORM_H

#include "Shiny/Defines.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Shiny {

struct SHINYAPI Transform {
   glm::quat orientation;
   glm::vec3 position;
   glm::vec3 scale;

   Transform()
      : orientation(), position(0.0f), scale(1.0f) {}
};

} // namespace Shiny

#endif
