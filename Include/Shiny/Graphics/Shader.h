#ifndef SHINY_SHADER_H
#define SHINY_SHADER_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class Shader {
protected:
   GLuint id;
   GLenum type;

   void release();

   void move(Shader &&other);

public:
   Shader(GLenum type);

   Shader(Shader &&other);

   Shader& operator=(Shader &&other);

   ~Shader();

   bool compile(const char *source);

   GLuint getID() const {
      return id;
   }

   GLenum getType() const {
      return type;
   }
};

} // namespace Shiny

#endif
