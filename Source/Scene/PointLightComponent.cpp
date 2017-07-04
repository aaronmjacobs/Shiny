#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Scene/PointLightComponent.h"

namespace Shiny {

void PointLightComponent::apply(ShaderProgram& program, RenderData& renderData) {
   LightComponent::apply(program, renderData);

   if (program.hasUniform("uLight.position")) {
      program.setUniformValue("uLight.position", getAbsoluteTransform().position);
   }

   if (program.hasUniform("uLight.squareFalloff")) {
      program.setUniformValue("uLight.squareFalloff", squareFalloff);
   }
}

SHINY_REGISTER_COMPONENT(PointLightComponent)

} // namespace Shiny
