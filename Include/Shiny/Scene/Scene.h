#ifndef SHINY_SCENE_H
#define SHINY_SCENE_H

#include "Shiny/Pointers.h"
#include "Shiny/Entity/Entity.h"

#include <algorithm>
#include <string>
#include <vector>

namespace Shiny {

class CameraComponent;

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
      auto itr = std::find_if(entities.begin(), entities.end(), [entityToDestroy](const UPtr<Entity>& entity) { return entity.get() == entityToDestroy; });

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

private:
   std::vector<UPtr<Entity>> entities;
   CameraComponent* activeCamera;
};

} // namespace Shiny

#endif
