#include "Shiny/Shiny.h"
#include "Shiny/Assets/ShaderLoader.h"
#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/Framebuffer.h"
#include "Shiny/Graphics/Mesh.h"
#include "Shiny/Graphics/Model.h"
#include "Shiny/Graphics/RenderData.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Graphics/TextureMaterial.h"
#include "Shiny/Graphics/Viewport.h"
#include "Shiny/Scene/CameraComponent.h"
#include "Shiny/Scene/DeferredRenderer.h"
#include "Shiny/Scene/ModelComponent.h"
#include "Shiny/Scene/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

namespace Shiny {

DeferredRenderer::DeferredRenderer(ShaderLoader& shaderLoader) {
   std::vector<Tex::InternalFormat> attachmentFormats = { Tex::InternalFormat::kRGB16F, Tex::InternalFormat::kRGB16F, Tex::InternalFormat::kRGBA8 };
   framebuffer = std::make_unique<Framebuffer>(0, 0, true, attachmentFormats);

   std::array<glm::vec3, 4> vertices = { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
   std::array<unsigned int, 6> indices = { 0, 1, 2, 1, 3, 2 };
   SPtr<Mesh> screenQuadMesh = std::make_shared<Mesh>(glm::value_ptr(vertices[0]), static_cast<unsigned int>(vertices.size()), nullptr, 0, nullptr, 0, indices.data(), static_cast<unsigned int>(indices.size()));
   
   SPtr<Shader> screenVertShader = shaderLoader.loadShader(Shiny::dataPath() + "/Screen.vert", GL_VERTEX_SHADER);
   SPtr<Shader> textureShader = shaderLoader.loadShader(Shiny::dataPath() + "/Texture.frag", GL_FRAGMENT_SHADER);
   SPtr<ShaderProgram> screenShaderProgram = std::make_shared<ShaderProgram>();
   screenShaderProgram->attach(screenVertShader);
   screenShaderProgram->attach(textureShader);
   bool linked = screenShaderProgram->link();
   ASSERT(linked);

   screenQuad.setMesh(screenQuadMesh);
   screenQuad.setShaderProgram(screenShaderProgram);
   screenQuad.attachMaterial(std::make_shared<TextureMaterial>(framebuffer->getColorAttachment(2), "uTexture"));
}

void DeferredRenderer::render(const Scene& scene) {
   CameraComponent* camera = scene.getActiveCamera();
   if (!camera) {
      return;
   }

   Viewport viewport = Context::current()->getViewport();
   float aspectRatio = static_cast<float>(viewport.width) / viewport.height;

   glm::mat4 projMatrix = glm::perspective(camera->getFov(), aspectRatio, 0.1f, 100.0f);
   glm::mat4 viewMatrix = camera->getViewMatrix();
   RenderData renderData;

   framebuffer->bind();

   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

   for (const auto& pair : scene.getModelComponentsByShaderProgram()) {
      ShaderProgram* shaderProgram = pair.first;

      if (shaderProgram) {
         shaderProgram->setUniformValue("uProjMatrix", projMatrix);
         shaderProgram->setUniformValue("uViewMatrix", viewMatrix);
      }

      for (ModelComponent* modelComponent : pair.second) {
         modelComponent->render(renderData);
      }
   }

   Framebuffer::bindDefaultFramebuffer();

   glDisable(GL_DEPTH_TEST);
   screenQuad.draw(renderData);
   glEnable(GL_DEPTH_TEST);
}

} // namespace Shiny
