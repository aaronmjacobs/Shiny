#ifndef SHINY_RENDER_DATA_H
#define SHINY_RENDER_DATA_H

#include "Shiny/Defines.h"
#include "Shiny/GLIncludes.h"

namespace Shiny {

class SHINYAPI RenderData {
private:
   static GLint maxTextureUnits();

   GLint nextTextureUnit;

public:
   RenderData();

   GLint aquireTextureUnit();
};

} // namespace Shiny

#endif
