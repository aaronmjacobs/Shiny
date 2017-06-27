#ifndef SHINY_MATH_UTILS_H
#define SHINY_MATH_UTILS_H

#include <glm/glm.hpp>

#include <cmath>

namespace Shiny {

namespace MathUtils {

namespace {

constexpr float kSmallNumber = 1.e-8f;

static float safeReciprocal(float val, float tolerance = kSmallNumber) {
   if (std::fabsf(val) <= tolerance) {
      return 0.0f;
   }

   return 1.0f / val;
}

static glm::vec3 safeReciprocal(const glm::vec3& val) {
   return glm::vec3(safeReciprocal(val.x), safeReciprocal(val.y), safeReciprocal(val.z));
}

} // namespace

} // namespace MathUtils

} // namespace Shiny

#endif
