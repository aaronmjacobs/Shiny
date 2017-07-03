#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Scene/ModelComponent.h"
#include "Shiny/Scene/Scene.h"

#include <glm/gtx/transform.hpp>

namespace Shiny {

namespace {

const char* kModelMatrix = "uModelMatrix";
const char* kNormalMatrix = "uNormalMatrix";

} // namespace

ModelComponent::ModelComponent(Entity& entity)
   : TransformComponent(entity) {
   getOwner().getScene().registerModelComponent(this);
}

void ModelComponent::render(RenderData renderData) {
   const SPtr<ShaderProgram>& program = model.getShaderProgram();

   if (program && program->hasUniform(kModelMatrix)) {
      Transform absoluteTransform = getAbsoluteTransform();
      glm::mat4 modelMatrix = absoluteTransform.toMatrix();
      program->setUniformValue(kModelMatrix, absoluteTransform.toMatrix());

      if (program->hasUniform(kNormalMatrix)) {
         glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
         program->setUniformValue(kNormalMatrix, normalMatrix);
      }
   }

   model.draw(renderData);
}

SHINY_REGISTER_COMPONENT(ModelComponent)

} // namespace Shiny
