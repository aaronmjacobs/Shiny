#include "Shiny/Graphics/ShaderProgram.h"

#include "Shiny/Scene/SceneObject.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace Shiny {

SceneObject::SceneObject(const SPtr<Scene> &scene)
   : scene(scene) {
}

SceneObject::~SceneObject() {
}

void SceneObject::draw(RenderData renderData) {
   const SPtr<ShaderProgram> &program(model.getProgram());
   const char *uModelMatrix = "uModelMatrix";

   if (program && program->hasUniform(uModelMatrix)) {
      glm::mat4 transMatrix(glm::translate(transform.position));
      glm::mat4 rotMatrix(glm::toMat4(transform.orientation));
      glm::mat4 scaleMatrix(glm::scale(transform.scale));
      glm::mat4 modelMatrix = transMatrix * rotMatrix * scaleMatrix;
      program->setUniformValue(uModelMatrix, modelMatrix);

      const char *uNormalMatrix = "uNormalMatrix";
      if (program->hasUniform(uNormalMatrix)) {
         glm::mat4 normalMatrix(glm::transpose(glm::inverse(modelMatrix)));
         program->setUniformValue(uNormalMatrix, normalMatrix);
      }
   }

   model.draw(renderData);
}

void SceneObject::setMesh(const SPtr<Mesh> &mesh) {
   model.setMesh(mesh);
}

void SceneObject::setShaderProgram(const SPtr<ShaderProgram> &program) {
   SPtr<ShaderProgram> oldProgram(model.getProgram());
   model.setShaderProgram(program);

   SPtr<Scene> sScene(scene.lock());
   if (sScene) {
      sScene->shaderProgramChange(shared_from_this(), oldProgram, program);
   }
}

void SceneObject::attachMaterial(const SPtr<Material> &material) {
   model.attachMaterial(material);
}

void SceneObject::removeMaterial(const SPtr<Material> &material) {
   model.removeMaterial(material);
}

const glm::vec3& SceneObject::getPosition() const {
   return transform.position;
}

const glm::quat& SceneObject::getOrientation() const {
   return transform.orientation;
}

const glm::vec3& SceneObject::getScale() const {
   return transform.scale;
}

void SceneObject::setPosition(const glm::vec3 &pos) {
   transform.position = pos;
}

void SceneObject::setOrientation(const glm::quat &ori) {
   transform.orientation = ori;
}

void SceneObject::setScale(const glm::vec3 &scale) {
   transform.scale = scale;
}

void SceneObject::translate(const glm::vec3 &trans) {
   transform.position += trans;
}

void SceneObject::rotate(float angle, const glm::vec3 &axis) {
   transform.orientation = glm::normalize(glm::angleAxis(angle, axis) * transform.orientation);
}

void SceneObject::scale(const glm::vec3 &amount) {
   transform.scale *= amount;
}

} // namespace Shiny
