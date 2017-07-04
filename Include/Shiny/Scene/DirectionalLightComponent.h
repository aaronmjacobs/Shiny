#ifndef SHINY_DIRECTIONAL_LIGHT_COMPONENT_H
#define SHINY_DIRECTIONAL_LIGHT_COMPONENT_H

#include "Shiny/Scene/LightComponent.h"

namespace Shiny {

class DirectionalLightComponent : public LightComponent {
public:
   virtual void apply(ShaderProgram& program, RenderData& renderData) override;

protected:
   friend class ComponentRegistrar<DirectionalLightComponent>;

   DirectionalLightComponent(Entity& entity)
      : LightComponent(entity) {
   }
};

SHINY_REFERENCE_COMPONENT(DirectionalLightComponent)

} // namespace Shiny

#endif
