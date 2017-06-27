#ifndef SHINY_MODEL_COMPONENT_H
#define SHINY_MODEL_COMPONENT_H

#include "Shiny/Graphics/Model.h"
#include "Shiny/Scene/TransformComponent.h"

namespace Shiny {

class ModelComponent : public TransformComponent {
public:
   void render(RenderData renderData);

   Model& getModel() {
      return model;
   }

   const Model& getModel() const {
      return model;
   }

protected:
   friend class ComponentRegistrar<ModelComponent>;

   ModelComponent(Entity& entity)
      : TransformComponent(entity) {
   }

private:
   Model model;
};

SHINY_REFERENCE_COMPONENT(ModelComponent)

} // namespace Shiny

#endif
