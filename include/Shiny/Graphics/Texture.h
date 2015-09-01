#ifndef SHINY_TEXTURE_H
#define SHINY_TEXTURE_H

#include "Shiny/Defines.h"
#include "Shiny/GLIncludes.h"

namespace Shiny {

class SHINYAPI Texture {
protected:
   GLuint textureID;
   GLenum target;

public:
   Texture(GLenum target);

   Texture(Texture &&other);

   virtual ~Texture();

   GLuint id() const {
      return textureID;
   }

   void bind();

   void unbind();
};

} // namespace Shiny

#endif
