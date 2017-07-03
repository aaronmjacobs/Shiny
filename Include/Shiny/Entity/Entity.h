#ifndef SHINY_ENTITY_H
#define SHINY_ENTITY_H

#include "Shiny/Pointers.h"
#include "Shiny/ShinyAssert.h"
#include "Shiny/Entity/Component.h"
#include "Shiny/Entity/Delegate.h"

#include <algorithm>
#include <string>
#include <vector>

namespace Shiny {

class Scene;

class Entity {
public:
   using OnDestroyDelegate = Delegate<void, Entity*>;

   Entity(const Entity& other) = delete;
   Entity(Entity&& other) = delete;

   ~Entity() {
      ASSERT(components.empty(), "Components vector not empty, was destroy() not called?");
   }

   Entity& operator=(const Entity& other) = delete;
   Entity& operator=(Entity&& other) = delete;

   template<typename T>
   T* createComponent() {
      components.push_back(ComponentRegistry::instance().createComponent<T>(*this));

      T* newComponent = static_cast<T*>(components.back().get());
      onComponentCreated(newComponent);
      return newComponent;
   }

   Component* createComponentByName(const std::string& className) {
      UPtr<Component> newComponent = ComponentRegistry::instance().createComponent(*this, className);
      if (!newComponent) {
         return nullptr;
      }

      Component* newComponentRaw = newComponent.get();
      components.push_back(std::move(newComponent));
      onComponentCreated(newComponentRaw);
      return newComponentRaw;
   }

   template<typename T>
   T* getComponentByClass() {
      for (const UPtr<Component>& component : components) {
         if (T* castedComponent = dynamic_cast<T*>(component.get())) {
            return castedComponent;
         }
      }

      return nullptr;
   }

   template<typename T>
   const T* getComponentByClass() const {
      for (const UPtr<Component>& component : components) {
         if (const T* castedComponent = dynamic_cast<const T*>(component.get())) {
            return castedComponent;
         }
      }

      return nullptr;
   }

   template<typename T>
   std::vector<T*> getComponentsByClass() {
      std::vector<T*> componentsOfClass;

      for (const UPtr<Component>& component : components) {
         if (T* castedComponent = dynamic_cast<T*>(component.get())) {
            componentsOfClass.push_back(castedComponent);
         }
      }

      return componentsOfClass;
   }

   template<typename T>
   std::vector<const T*> getComponentsByClass() const {
      std::vector<const T*> componentsOfClass;

      for (const UPtr<Component>& component : components) {
         if (const T* castedComponent = dynamic_cast<const T*>(component.get())) {
            componentsOfClass.push_back(castedComponent);
         }
      }

      return componentsOfClass;
   }

   void destroy();

   OnDestroyDelegate::Handle bindOnDestroy(const OnDestroyDelegate::FuncType& function) {
      return onDestroy.bind(function);
   }

   bool destroyComponent(Component* componentToDestroy) {
      auto itr = std::find_if(components.begin(), components.end(), [componentToDestroy](const UPtr<Component>& component) { return component.get() == componentToDestroy; });

      if (itr != components.end()) {
         (*itr)->executeDestroy();
         components.erase(itr);
         return true;
      }

      return false;
   }

   Scene& getScene() {
      return scene;
   }

   const Scene& getScene() const {
      return scene;
   }

private:
   friend class Scene;

   template<typename... ComponentTypes>
   static UPtr<Entity> create(Scene& scene) {
      UPtr<Entity> entity(new Entity(scene));

      entity->constructComponents<ComponentTypes...>();
      entity->onInitialized();

      return entity;
   }

   static UPtr<Entity> create(const std::vector<std::string>& componentClassNames, Scene& scene) {
      UPtr<Entity> entity(new Entity(scene));

      for (const std::string& className : componentClassNames) {
         UPtr<Component> newComponent = ComponentRegistry::instance().createComponent(*entity, className);
         if (newComponent) {
            entity->components.push_back(std::move(newComponent));
         }
      }
      entity->onInitialized();

      return entity;
   }

   Entity(Scene& inScene)
      : scene(inScene) {
   }

   template<typename First, typename... Rest>
   void constructComponentsHelper() {
      components.push_back(ComponentRegistry::instance().createComponent<First>(*this));
      constructComponents<Rest...>();
   }

   template<typename... ComponentTypes>
   void constructComponents() {
      constructComponentsHelper<ComponentTypes...>();
   }

   void onInitialized() {
      for (const UPtr<Component>& component : components) {
         component->onOwnerInitialized();
      }
   }

   void onComponentCreated(Component* component) {
      for (const UPtr<Component>& component : components) {
         component->onComponentAddedToOwner(component.get());
      }
   }

   void executeDestroy() {
      for (const auto& component : components) {
         component->executeDestroy();
      }
      components.clear();

      onDestroy.execute(this);
   }

   std::vector<UPtr<Component>> components;
   Scene& scene;
   OnDestroyDelegate onDestroy;
};

template<>
inline void Entity::constructComponents() {
}

} // namespace Shiny

#endif
