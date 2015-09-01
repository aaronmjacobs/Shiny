#ifndef SHINY_CAMERA_H
#define SHINY_CAMERA_H

#include "Shiny/Defines.h"

#include "Shiny/Scene/Transform.h"

#include <glm/glm.hpp>

namespace Shiny {

class SHINYAPI Camera {
private:
   Transform transform;

public:
   const glm::vec3& getPosition() const {
      return transform.position;
   }

   const glm::quat& getOrientation() const {
      return transform.orientation;
   }

   glm::vec3 getFront() const;

   glm::vec3 getRight() const;

   glm::vec3 getUp() const;

   glm::mat4 getViewMatrix() const;

   void setPosition(const glm::vec3 &pos);

   void setOrientation(const glm::quat &ori);

   void fly(float amount);

   void strafe(float amount);

   void rotate(float pitch, float yaw);

   void lookAt(const glm::vec3 &loc);
};

} // namespace Shiny

#endif
