#ifndef SHINY_POINT_LIGHT_COMPONENT_H
#define SHINY_POINT_LIGHT_COMPONENT_H

#include "Shiny/Scene/LightComponent.h"

namespace Shiny {

class PointLightComponent : public LightComponent {
public:
   virtual void apply(ShaderProgram& program, RenderData& renderData) override;

   float getSquareFalloff() const {
      return squareFalloff;
   }

   void setSquareFalloff(float newSquareFalloff) {
      squareFalloff = newSquareFalloff;
   }

protected:
   friend class ComponentRegistrar<PointLightComponent>;

   PointLightComponent(Entity& entity)
      : LightComponent(entity), squareFalloff(1.0f) {
   }

private:
   float squareFalloff;
};

SHINY_REFERENCE_COMPONENT(PointLightComponent)

} // namespace Shiny

#endif
