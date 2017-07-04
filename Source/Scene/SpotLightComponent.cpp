#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Scene/SpotLightComponent.h"

namespace Shiny {

void SpotLightComponent::apply(ShaderProgram& program, RenderData& renderData) {
   LightComponent::apply(program, renderData);

   Transform absoluteTransform = getAbsoluteTransform();

   if (program.hasUniform("uLight.position")) {
      program.setUniformValue("uLight.position", absoluteTransform.position);
   }

   if (program.hasUniform("uLight.direction")) {
      program.setUniformValue("uLight.direction", absoluteTransform.orientation * glm::vec3(0.0f, 0.0f, -1.0f));
   }

   if (program.hasUniform("uLight.squareFalloff")) {
      program.setUniformValue("uLight.squareFalloff", squareFalloff);
   }

   if (program.hasUniform("uLight.beamAngle")) {
      program.setUniformValue("uLight.beamAngle", beamAngle);
   }

   if (program.hasUniform("uLight.cutoffAngle")) {
      program.setUniformValue("uLight.cutoffAngle", squareFalloff);
   }
}

SHINY_REGISTER_COMPONENT(SpotLightComponent)

} // namespace Shiny
