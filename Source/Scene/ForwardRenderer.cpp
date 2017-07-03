#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/RenderData.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Graphics/Viewport.h"
#include "Shiny/Scene/CameraComponent.h"
#include "Shiny/Scene/ForwardRenderer.h"
#include "Shiny/Scene/ModelComponent.h"
#include "Shiny/Scene/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Shiny {

namespace {

const char* kProjMatrix = "uProjMatrix";
const char* kViewMatrix = "uViewMatrix";

} // namespace

void ForwardRenderer::render(const Scene& scene) {
   CameraComponent* camera = scene.getActiveCamera();
   if (!camera) {
      return;
   }

   Viewport viewport = Context::current()->getViewport();
   float aspectRatio = static_cast<float>(viewport.width) / viewport.height;

   glm::mat4 projMatrix = glm::perspective(camera->getFov(), aspectRatio, 0.1f, 100.0f);
   glm::mat4 viewMatrix = camera->getViewMatrix();
   RenderData renderData;

   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

   for (const auto& pair : scene.getModelComponentsByShaderProgram()) {
      ShaderProgram* shaderProgram = pair.first;

      if (shaderProgram) {
         shaderProgram->setUniformValue(kProjMatrix, projMatrix);
         shaderProgram->setUniformValue(kViewMatrix, viewMatrix);
      }

      for (ModelComponent* modelComponent : pair.second) {
         modelComponent->render(renderData);
      }
   }
}

} // namespace Shiny
