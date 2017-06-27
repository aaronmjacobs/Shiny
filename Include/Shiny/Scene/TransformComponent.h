#ifndef SHINY_TRANSFORM_COMPONENT_H
#define SHINY_TRANSFORM_COMPONENT_H

#include "Shiny/Entity/Component.h"
#include "Shiny/Math/Transform.h"

namespace Shiny {

class TransformComponent : public Component {
public:
   TransformComponent* getParent() {
      return parent;
   }

   const TransformComponent* getParent() const {
      return parent;
   }

   void setParent(TransformComponent* newParent) {
      if (newParent && &getOwner() != &newParent->getOwner()) {
         return;
      }

      parent = newParent;
   }

   Transform& getRelativeTransform() {
      return relativeTransform;
   }

   const Transform& getRelativeTransform() const {
      return relativeTransform;
   }

   void setRelativeTransform(const Transform& newRelativeTransform) {
      relativeTransform = newRelativeTransform;
   }

   Transform getAbsoluteTransform() const {
      if (!parent) {
         return relativeTransform;
      }

      return parent->getAbsoluteTransform() * relativeTransform;
   }

   void setAbsoluteTransform(const Transform& newAbsoluteTransform) {
      if (!parent) {
         relativeTransform = newAbsoluteTransform;
         return;
      }

      relativeTransform *= parent->getAbsoluteTransform().inverse();
   }

protected:
   friend class ComponentRegistrar<TransformComponent>;

   TransformComponent(Entity& entity)
      : Component(entity), parent(nullptr) {
   }

protected:
   Transform relativeTransform;

private:
   TransformComponent* parent;
};

SHINY_REFERENCE_COMPONENT(TransformComponent)

} // namespace Shiny

#endif
