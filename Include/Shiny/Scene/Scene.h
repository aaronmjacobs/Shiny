#ifndef SHINY_SCENE_H
#define SHINY_SCENE_H

#include "Shiny/Pointers.h"
#include "Shiny/Entity/Entity.h"
#include "Shiny/Scene/ModelComponent.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace Shiny {

class CameraComponent;
class ShaderProgram;

class Scene {
public:
   Scene()
      : activeCamera(nullptr) {
   }

   ~Scene() {
      for (const auto& entity : entities) {
         entity->executeDestroy();
      }
      entities.clear();
   }

   template<typename... ComponentTypes>
   Entity* createEntity() {
      entities.push_back(Entity::create<ComponentTypes...>(*this));
      return entities.back().get();
   }

   Entity* createEntity(const std::vector<std::string>& componentClassNames) {
      entities.push_back(Entity::create(componentClassNames, *this));
      return entities.back().get();
   }

   const std::vector<UPtr<Entity>>& getEntities() const {
      return entities;
   }

   bool destroyEntity(Entity* entityToDestroy) {
      auto itr = std::find_if(entities.begin(), entities.end(), [entityToDestroy](const UPtr<Entity>& entity) {
         return entity.get() == entityToDestroy;
      });

      if (itr != entities.end()) {
         (*itr)->executeDestroy();
         entities.erase(itr);
         return true;
      }

      return false;
   }

   void setActiveCamera(CameraComponent* newCamera) {
      activeCamera = newCamera;
   }

   CameraComponent* getActiveCamera() const {
      return activeCamera;
   }

   void registerModelComponent(ModelComponent* modelComponent);

   const std::unordered_map<ShaderProgram*, std::vector<ModelComponent*>>& getModelComponentsByShaderProgram() const {
      return modelComponentsByShaderProgram;
   }

private:
   struct ModelComponentHandles {
      Component::OnDestroyDelegate::Handle onDestroyHandle;
      ModelComponent::OnShaderProgramChangeDelegate::Handle onShaderProgramChangeHandle;
   };

   std::vector<UPtr<Entity>> entities;
   std::unordered_map<ShaderProgram*, std::vector<ModelComponent*>> modelComponentsByShaderProgram;
   std::unordered_map<ModelComponent*, ModelComponentHandles> modelComponentHandles;
   CameraComponent* activeCamera;
};

} // namespace Shiny

#endif
