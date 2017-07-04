#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Scene/LightComponent.h"
#include "Shiny/Scene/Scene.h"

namespace Shiny {

LightComponent::LightComponent(Entity& entity)
   : TransformComponent(entity), color(1.0f) {
   getOwner().getScene().registerLightComponent(this);
}

void LightComponent::apply(ShaderProgram& program, RenderData& renderData) {
   if (program.hasUniform("uLight.color")) {
      program.setUniformValue("uLight.color", color);
   }
}

SHINY_REGISTER_COMPONENT(LightComponent)

} // namespace Shiny
