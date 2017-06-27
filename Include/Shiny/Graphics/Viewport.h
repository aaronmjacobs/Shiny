#ifndef SHINY_VIEWPORT_H
#define SHINY_VIEWPORT_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

struct Viewport {
   Viewport(GLint inX = 0, GLint inY = 0, GLint inWidth = 0, GLint inHeight = 0)
      : x(inX), y(inY), width(inWidth), height(inHeight) {
   }

   GLint* data() {
      return &x;
   }

   GLint x;
   GLint y;
   GLint width;
   GLint height;
};

} // namespace Shiny

#endif
