#ifndef SHINY_SCENE_OBJECT_H
#define SHINY_SCENE_OBJECT_H

#include "Shiny/Pointers.h"

#include "Shiny/Graphics/Model.h"

#include "Shiny/Scene/Scene.h"
#include "Shiny/Scene/Transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Shiny {

class SceneObject : public std::enable_shared_from_this<SceneObject> {
private:
   WPtr<Scene> scene;
   Model model;
   Transform transform;

   SceneObject(const SPtr<Scene> &scene);

   friend SPtr<SceneObject> Scene::createObject();

public:
   virtual ~SceneObject() = default;

   void draw(RenderData renderData);

   const Model& getModel() const {
      return model;
   }

   void setMesh(const SPtr<Mesh> &mesh);

   void setShaderProgram(const SPtr<ShaderProgram> &program);

   void attachMaterial(const SPtr<Material> &material);

   void removeMaterial(const SPtr<Material> &material);

   const glm::vec3& getPosition() const;

   const glm::quat& getOrientation() const;

   const glm::vec3& getScale() const;

   void setPosition(const glm::vec3 &pos);

   void setOrientation(const glm::quat &ori);

   void setScale(const glm::vec3 &scale);

   void translate(const glm::vec3 &trans);

   void rotate(float angle, const glm::vec3 &axis);

   void scale(const glm::vec3 &amount);
};

} // namespace Shiny

#endif
