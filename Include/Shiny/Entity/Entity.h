#ifndef SHINY_ENTITY_H
#define SHINY_ENTITY_H

#include "Shiny/Pointers.h"
#include "Shiny/Entity/Component.h"

#include <string>
#include <vector>

namespace Shiny {

class Entity {
public:
   template<typename EntityType, typename... ComponentTypes, typename... ConstructorArgs>
   static UPtr<EntityType> create(ConstructorArgs&&... constructorArgs) {
      UPtr<EntityType> entity(new EntityType(std::forward<ConstructorArgs>(constructorArgs)...));

      entity->constructComponents<ComponentTypes...>();
      entity->onInitialized();

      return entity;
   }

   template<typename EntityType, typename... ConstructorArgs>
   static UPtr<EntityType> create(const std::vector<std::string>& componentClassNames, ConstructorArgs&&... constructorArgs) {
      UPtr<EntityType> entity(new EntityType(std::forward<ConstructorArgs>(constructorArgs)...));

      for (const std::string& className : componentClassNames) {
         UPtr<Component> newComponent = ComponentRegistry::instance().createComponent(*entity, className);
         if (newComponent) {
            entity->components.push_back(std::move(newComponent));
         }
      }
      entity->onInitialized();

      return entity;
   }

   template<typename EntityType, typename... ComponentTypes, typename... ConstructorArgs>
   static SPtr<EntityType> createShared(ConstructorArgs&&... constructorArgs) {
      return SPtr<EntityType>(create<EntityType, ComponentTypes...>(std::forward<ConstructorArgs>(constructorArgs)...).release());
   }

   template<typename EntityType, typename... ConstructorArgs>
   static SPtr<EntityType> createShared(const std::vector<std::string>& componentClassNames, ConstructorArgs&&... constructorArgs) {
      return SPtr<EntityType>(create<EntityType>(componentClassNames, std::forward<ConstructorArgs>(constructorArgs)...).release());
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

protected:
   Entity() = default;

private:
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
