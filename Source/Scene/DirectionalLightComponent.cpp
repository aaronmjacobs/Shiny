#include "Shiny/Scene/DirectionalLightComponent.h"

namespace Shiny {

void DirectionalLightComponent::apply(ShaderProgram& program, RenderData& renderData) {
   LightComponent::apply(program, renderData);
}

SHINY_REGISTER_COMPONENT(DirectionalLightComponent)

} // namespace Shiny
