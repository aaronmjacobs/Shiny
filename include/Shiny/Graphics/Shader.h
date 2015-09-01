#ifndef SHINY_SHADER_H
#define SHINY_SHADER_H

#include "Shiny/GLIncludes.h"

namespace Shiny {

class Shader {
protected:
   GLuint id;
   const GLenum type;

public:
   Shader(const GLenum type);

   Shader(Shader &&other);

   virtual ~Shader();

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
