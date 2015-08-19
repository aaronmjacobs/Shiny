#include "Shader.h"

#include "ShinyAssert.h"

namespace Shiny {

Shader::Shader(const GLenum type)
   : id(glCreateShader(type)), type(type) {
   ASSERT(type == GL_VERTEX_SHADER ||
          type == GL_FRAGMENT_SHADER ||
          type == GL_GEOMETRY_SHADER,
          "Invalid shader type: %u", type);
}

Shader::Shader(Shader &&other)
   : id(other.id), type(other.type) {
   other.id = 0;
}

Shader::~Shader() {
   glDeleteShader(id);
}

bool Shader::compile(const char *source) {
   glShaderSource(id, 1, &source, nullptr);
   glCompileShader(id);

   GLint status;
   glGetShaderiv(id, GL_COMPILE_STATUS, &status);
   return status == GL_TRUE;
}

} // namespace Shiny
