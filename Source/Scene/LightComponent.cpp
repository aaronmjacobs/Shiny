#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Scene/LightComponent.h"

namespace Shiny {

void LightComponent::apply(ShaderProgram& program, RenderData& renderData) {
   if (program.hasUniform("uLight.color")) {
      program.setUniformValue("uLight.color", color);
   }
}

SHINY_REGISTER_COMPONENT(LightComponent)

} // namespace Shiny
