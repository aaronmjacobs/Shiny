#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Shader.h"

namespace Shiny {

Shader::Shader(GLenum type)
   : id(glCreateShader(type)), type(type) {
   ASSERT(type == GL_VERTEX_SHADER ||
          type == GL_FRAGMENT_SHADER ||
          type == GL_GEOMETRY_SHADER,
          "Invalid shader type: %u", type);
}

Shader::Shader(Shader &&other) {
   move(std::move(other));
}

Shader& Shader::operator=(Shader &&other) {
   release();
   move(std::move(other));
   return *this;
}

Shader::~Shader() {
   release();
}

void Shader::release() {
   glDeleteShader(id);
}

void Shader::move(Shader &&other) {
   id = other.id;
   type = other.type;

   other.id = 0;
}

bool Shader::compile(const char *source) {
   glShaderSource(id, 1, &source, nullptr);
   glCompileShader(id);

   GLint status;
   glGetShaderiv(id, GL_COMPILE_STATUS, &status);
   return status == GL_TRUE;
}

} // namespace Shiny
