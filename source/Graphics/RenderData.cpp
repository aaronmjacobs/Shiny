#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/RenderData.h"

namespace Shiny {

GLint RenderData::maxTextureUnits() {
   static GLint maxUnits = -1;

   if (maxUnits < 0) {
      glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
      ASSERT(maxUnits > 0, "Max texture units is less than 1");
   }

   return maxUnits;
}

GLint RenderData::aquireTextureUnit() {
   ASSERT(nextTextureUnit < maxTextureUnits(), "Ran out of texture units");
   return nextTextureUnit++;
}

} // namespace Shiny
