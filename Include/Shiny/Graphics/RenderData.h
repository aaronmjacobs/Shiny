#ifndef SHINY_RENDER_DATA_H
#define SHINY_RENDER_DATA_H

#include "Shiny/ShinyAssert.h"
#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class ShaderProgram;

class RenderData {
public:
   RenderData()
      : nextTextureUnit(0), overrideProgram(nullptr) {
   }

   GLint aquireTextureUnit() {
      ASSERT(nextTextureUnit < maxTextureUnits(), "Ran out of texture units");
      return nextTextureUnit++;
   }

   ShaderProgram* getOverrideProgram() const {
      return overrideProgram;
   }

   void setOverrideProgram(ShaderProgram* newOverrideProgram) {
      overrideProgram = newOverrideProgram;
   }

private:
   static GLint maxTextureUnits();

   GLint nextTextureUnit;
   ShaderProgram* overrideProgram;
};

} // namespace Shiny

#endif
