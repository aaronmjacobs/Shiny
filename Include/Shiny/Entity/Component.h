#ifndef SHINY_COMPONENT_H
#define SHINY_COMPONENT_H

#include "Shiny/Pointers.h"
#include "Shiny/ShinyAssert.h"

#include <string>
#include <unordered_map>

namespace Shiny {

class Entity;

class Component {
public:
   virtual ~Component() = default;

   virtual void onOwnerInitialized() {
   }

   virtual void onComponentAddedToOwner(Component* component) {
   }

   Entity& getOwner() {
      return owner;
   }

   const Entity& getOwner() const {
      return owner;
   }

protected:
   Component(Entity& entity)
      : owner(entity) {
   }

private:
   Entity& owner;
};

template<typename T>
class ComponentRegistrar {
public:
   ComponentRegistrar(const std::string& className);
   ~ComponentRegistrar();

private:
   friend class ComponentRegistry;

   static UPtr<T> createTypedComponent(Entity& entity) {
      return UPtr<T>(new T(entity));
   }

   static UPtr<Component> createComponent(Entity& entity) {
      return createTypedComponent(entity);
   }

   std::string componentClassName;
};

#define SHINY_REGISTER_COMPONENT(component_name)\
static const ComponentRegistrar<component_name> kShiny##component_name##Registrar(#component_name)

class ComponentRegistry {
public:
   static ComponentRegistry& instance();

   ComponentRegistry(const ComponentRegistry& other) = delete;
   ComponentRegistry(ComponentRegistry&& other) = delete;
   ComponentRegistry& operator=(const ComponentRegistry& other) = delete;
   ComponentRegistry& operator=(ComponentRegistry&& other) = delete;

private:
   template<typename T> friend class ComponentRegistrar;
   friend class Entity;

   using CreateComponentFunc = UPtr<Component>(*)(Entity&);

   ComponentRegistry() = default;

   void registerComponent(const std::string& className, CreateComponentFunc createComponentFunc) {
      auto pair = componentMap.insert({ className, createComponentFunc });
      ASSERT(pair.second, "Trying to register a component that has already been registered!");
   }

   void unregisterComponent(const std::string& className) {
      auto itr = componentMap.find(className);
      ASSERT(itr != componentMap.end(), "Trying to unregister a component that has not been registered!");

      componentMap.erase(itr);
   }

   template<typename T>
   UPtr<T> createComponent(Entity& entity) {
      return ComponentRegistrar<T>::createTypedComponent(entity);
   }

   UPtr<Component> createComponent(Entity& entity, const std::string& className) {
      auto itr = componentMap.find(className);
      if (itr != componentMap.end()) {
         return itr->second(entity);
      }

      return nullptr;
   }

   std::unordered_map<std::string, CreateComponentFunc> componentMap;
};

template<typename T>
ComponentRegistrar<T>::ComponentRegistrar(const std::string& className)
   : componentClassName(className) {
   ComponentRegistry::instance().registerComponent(componentClassName, &createComponent);
}

template<typename T>
ComponentRegistrar<T>::~ComponentRegistrar() {
   // Because the ComponentRegistry singleton will complete construction before any of the ComponentRegistrars
   // (since it is created inside the above constructor), it will not be destroyed until after all
   // ComponentRegistrars are destroyed. Therefore, it is safe to access it in this destructor.
   ComponentRegistry::instance().unregisterComponent(componentClassName);
}

} // namespace Shiny

#endif
