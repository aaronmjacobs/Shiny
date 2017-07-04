#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Scene/DirectionalLightComponent.h"

namespace Shiny {

void DirectionalLightComponent::apply(ShaderProgram& program, RenderData& renderData) {
   LightComponent::apply(program, renderData);

   if (program.hasUniform("uLight.direction")) {
      program.setUniformValue("uLight.direction", getAbsoluteTransform().orientation * glm::vec3(0.0f, 0.0f, -1.0f));
   }
}

SHINY_REGISTER_COMPONENT(DirectionalLightComponent)

} // namespace Shiny
