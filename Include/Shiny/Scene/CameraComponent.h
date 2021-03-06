#ifndef SHINY_CAMERA_COMPONENT_H
#define SHINY_CAMERA_COMPONENT_H

#include "Shiny/Scene/TransformComponent.h"

namespace Shiny {

class CameraComponent : public TransformComponent {
public:
   glm::vec3 getFront() const;
   glm::vec3 getRight() const;
   glm::vec3 getUp() const;
   glm::mat4 getViewMatrix() const;

   float getFov() const {
      return fov;
   }

   void setFov(float newFov) {
      fov = newFov;
   }

   void fly(float amount);
   void strafe(float amount);
   void rotate(float pitch, float yaw);
   void lookAt(const glm::vec3 &loc);

protected:
   friend class ComponentRegistrar<CameraComponent>;

   CameraComponent(Entity& entity)
      : TransformComponent(entity), fov(70.0f) {
   }

private:
   float fov;
};

SHINY_REFERENCE_COMPONENT(CameraComponent)

} // namespace Shiny

#endif
