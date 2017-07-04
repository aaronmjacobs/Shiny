#ifndef SHINY_LIGHT_COMPONENT_H
#define SHINY_LIGHT_COMPONENT_H

#include "Shiny/Scene/TransformComponent.h"

#include <glm/glm.hpp>

namespace Shiny {

class ShaderProgram;
class RenderData;

class LightComponent : public TransformComponent {
public:
   virtual void apply(ShaderProgram& program, RenderData& renderData);

   const glm::vec3& getColor() const {
      return color;
   }

   void setColor(const glm::vec3& newColor) {
      color = newColor;
   }

protected:
   friend class ComponentRegistrar<LightComponent>;

   LightComponent(Entity& entity);

private:
   glm::vec3 color;
};

SHINY_REFERENCE_COMPONENT(LightComponent)

} // namespace Shiny

#endif
