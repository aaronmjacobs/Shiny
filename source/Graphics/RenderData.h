#ifndef SHINY_RENDER_DATA_H
#define SHINY_RENDER_DATA_H

#include "GLIncludes.h"

namespace Shiny {

class RenderData {
private:
   static GLint maxTextureUnits();

   GLint nextTextureUnit;

public:
   RenderData();

   GLint aquireTextureUnit();
};

} // namespace Shiny

#endif
