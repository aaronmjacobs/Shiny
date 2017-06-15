#ifndef SHINY_ENTITY_H
#define SHINY_ENTITY_H

#include "Shiny/Pointers.h"
#include "Shiny/Entity/Component.h"

#include <string>
#include <vector>

namespace Shiny {

class Entity {
public:
   template<typename... ComponentTypes>
   static UPtr<Entity> create() {
      UPtr<Entity> entity(new Entity);

      entity->constructComponents<ComponentTypes...>();
      entity->onInitialized();

      return entity;
   }

   template<typename... ComponentTypes>
   static SPtr<Entity> createShared() {
      return SPtr<Entity>(create<ComponentTypes...>().release());
   }

   Entity(const Entity& other) = delete;
   Entity(Entity&& other) = delete;
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
   std::vector<T*> getComponentsByClass() {
      std::vector<T*> componentsOfClass;

      for (const UPtr<Component>& component : components) {
         if (T* castedComponent = dynamic_cast<T*>(component.get())) {
            componentsOfClass.push_back(castedComponent);
         }
      }

      return componentsOfClass;
   }

private:
   Entity() = default;

   template<typename First, typename... Rest>
   void constructComponentsHelper() {
      components.push_back(ComponentRegistry::instance().createComponent<First>(*this));
      constructComponents<Rest...>();
   }

   template<typename... ComponentTypes>
   void constructComponents() {
      constructComponentsHelper<ComponentTypes...>();
   }

   template<>
   void constructComponents() {
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

   std::vector<UPtr<Component>> components;
};

} // namespace Shiny

#endif
