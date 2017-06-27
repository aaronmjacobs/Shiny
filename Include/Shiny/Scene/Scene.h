#ifndef SHINY_SCENE_H
#define SHINY_SCENE_H

#include "Shiny/Pointers.h"
#include "Shiny/Entity/Entity.h"

#include <string>
#include <vector>

namespace Shiny {

class CameraComponent;

class Scene {
public:
   Scene()
      : activeCamera(nullptr) {
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
