#ifndef SHINY_SPOT_LIGHT_COMPONENT_H
#define SHINY_SPOT_LIGHT_COMPONENT_H

#include "Shiny/Scene/LightComponent.h"

namespace Shiny {

class SpotLightComponent : public LightComponent {
public:
   virtual void apply(ShaderProgram& program, RenderData& renderData) override;

   float getSquareFalloff() const {
      return squareFalloff;
   }

   void setSquareFalloff(float newSquareFalloff) {
      squareFalloff = newSquareFalloff;
   }

   float getBeamAngle() const {
      return beamAngle;
   }

   void setBeamAngle(float newBeamAngle) {
      beamAngle = newBeamAngle;
   }

   float getCutoffAngle() const {
      return cutoffAngle;
   }

   void setCutoffAngle(float newCutoffAngle) {
      cutoffAngle = newCutoffAngle;
   }

protected:
   friend class ComponentRegistrar<SpotLightComponent>;

   SpotLightComponent(Entity& entity)
      : LightComponent(entity), squareFalloff(1.0f), beamAngle(glm::quarter_pi<float>()),
        cutoffAngle(glm::half_pi<float>()) {
   }

private:
   float squareFalloff;
   float beamAngle;
   float cutoffAngle;
};

SHINY_REFERENCE_COMPONENT(SpotLightComponent)

} // namespace Shiny

#endif
