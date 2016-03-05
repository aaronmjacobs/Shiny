#ifndef SHINY_TEXTURE_H
#define SHINY_TEXTURE_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class Texture {
protected:
   GLuint textureID { 0 };
   GLenum target;

   void release();

   void move(Texture &&other);

public:
   Texture(GLenum target);

   Texture(Texture &&other);

   Texture& operator=(Texture &&other);

   ~Texture();

   GLuint id() const {
      return textureID;
   }

   void bind();

   void unbind();
};

} // namespace Shiny

#endif
