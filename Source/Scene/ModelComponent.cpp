#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Scene/ModelComponent.h"
#include "Shiny/Scene/Scene.h"

#include <glm/gtx/quaternion.hpp>
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

      glm::mat4 transMatrix(glm::translate(absoluteTransform.position));
      glm::mat4 rotMatrix(glm::toMat4(absoluteTransform.orientation));
      glm::mat4 scaleMatrix(glm::scale(absoluteTransform.scale));
      glm::mat4 modelMatrix = transMatrix * rotMatrix * scaleMatrix;
      program->setUniformValue(kModelMatrix, modelMatrix);

      if (program->hasUniform(kNormalMatrix)) {
         glm::mat4 normalMatrix(glm::transpose(glm::inverse(modelMatrix)));
         program->setUniformValue(kNormalMatrix, normalMatrix);
      }
   }

   model.draw(renderData);
}

SHINY_REGISTER_COMPONENT(ModelComponent)

} // namespace Shiny
