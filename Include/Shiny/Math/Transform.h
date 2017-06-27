#ifndef SHINY_TRANSFORM_H
#define SHINY_TRANSFORM_H

#include "Shiny/Math/MathUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Shiny {

struct Transform {
   glm::quat orientation;
   glm::vec3 position;
   glm::vec3 scale;

   Transform(const glm::quat& inOrientation = {}, const glm::vec3& inPosition = glm::vec3(0.0f), const glm::vec3& inScale = glm::vec3(1.0f))
      : orientation(inOrientation), position(inPosition), scale(inScale) {
   }

   Transform inverse() const {
      glm::quat inverseOrientation = glm::inverse(orientation);
      glm::vec3 inverseScale = MathUtils::safeReciprocal(scale);
      glm::vec3 inversePosition = inverseOrientation * (inverseScale * -position);

      return Transform(inverseOrientation, inversePosition, inverseScale);
   }

   Transform operator*(const Transform& other) const {
      Transform result;
      multiply(result, *this, other);
      return result;
   }

   void operator*=(const Transform& other) {
      multiply(*this, *this, other);
   }

private:
   static void multiply(Transform& result, const Transform& first, const Transform& second) {
      result.orientation = second.orientation * first.orientation;
      result.scale = second.scale * first.scale;
      result.position = second.orientation * (second.scale * first.position) + second.position;
   }
};

} // namespace Shiny

#endif
