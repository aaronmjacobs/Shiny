#ifndef SHINY_RENDER_DATA_H
#define SHINY_RENDER_DATA_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class RenderData {
private:
   static GLint maxTextureUnits();

   GLint nextTextureUnit { 0 };

public:
   GLint aquireTextureUnit();
};

} // namespace Shiny

#endif
