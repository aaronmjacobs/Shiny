#include "ShaderProgram.h"

#include "Log.h"
#include "Shader.h"
#include "ShinyAssert.h"

#include <cstring>

namespace Shiny {

namespace {

void bindAttribute(GLuint id, GLint attribute) {
   glBindAttribLocation(id, attribute, ShaderAttributes::NAMES[attribute]);
}

} // namespace

// Uniform

Uniform::Uniform(const GLint location, const GLenum type, const std::string &name)
   : data({0}), name(name), location(location), type(type), dirty(false) {
}

void Uniform::commit() {
   if (!dirty) {
      return;
   }

   switch (type) {
      case GL_BOOL:
         glUniform1i(location, data.boolVal);
         break;
      case GL_INT:
      case GL_SAMPLER_1D:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_3D:
      case GL_SAMPLER_CUBE:
      case GL_SAMPLER_1D_SHADOW:
      case GL_SAMPLER_2D_SHADOW:
      case GL_SAMPLER_CUBE_SHADOW:
         glUniform1i(location, data.intVal);
         break;
      case GL_FLOAT:
         glUniform1f(location, data.floatVal);
         break;
      case GL_FLOAT_VEC2:
         glUniform2fv(location, 1, SHINY_RAW_VALUE(data.vec2Val));
         break;
      case GL_FLOAT_VEC3:
         glUniform3fv(location, 1, SHINY_RAW_VALUE(data.vec3Val));
         break;
      case GL_FLOAT_VEC4:
         glUniform4fv(location, 1, SHINY_RAW_VALUE(data.vec4Val));
         break;
      case GL_FLOAT_MAT4:
         glUniformMatrix4fv(location, 1, GL_FALSE, SHINY_RAW_VALUE(data.mat4Val));
         break;
      default:
         ASSERT(false, "Invalid uniform type: %d", type);
         break;
   }

   dirty = false;
}

void Uniform::setValue(bool value) {
   ASSERT(type == GL_BOOL);
   dirty = dirty || data.boolVal != value;
   data.boolVal = value;
}

void Uniform::setValue(int value) {
   ASSERT(type == GL_INT ||
          type == GL_SAMPLER_1D ||
          type == GL_SAMPLER_2D ||
          type == GL_SAMPLER_3D ||
          type == GL_SAMPLER_CUBE ||
          type == GL_SAMPLER_1D_SHADOW ||
          type == GL_SAMPLER_2D_SHADOW ||
          type == GL_SAMPLER_CUBE_SHADOW);
   dirty = dirty || data.intVal != value;
   data.intVal = value;
}

void Uniform::setValue(GLenum value) {
   setValue((int)value);
}

void Uniform::setValue(float value) {
   ASSERT(type == GL_FLOAT);
   dirty = dirty || data.floatVal != value;
   data.floatVal = value;
}

void Uniform::setValue(const vec2 &value) {
   ASSERT(type == GL_FLOAT_VEC2);
   dirty = dirty || data.vec2Val != value;
   data.vec2Val = value;
}

void Uniform::setValue(const vec3 &value) {
   ASSERT(type == GL_FLOAT_VEC3);
   dirty = dirty || data.vec3Val != value;
   data.vec3Val = value;
}

void Uniform::setValue(const vec4 &value) {
   ASSERT(type == GL_FLOAT_VEC4);
   dirty = dirty || data.vec4Val != value;
   data.vec4Val = value;
}

void Uniform::setValue(const mat4 &value) {
   ASSERT(type == GL_FLOAT_MAT4);
   dirty = dirty || data.mat4Val != value;
   data.mat4Val = value;
}

// ShaderProgram

ShaderProgram::ShaderProgram()
   : id(glCreateProgram()) {
   for (int i = 0; i < ShaderAttributes::NAMES.size(); ++i) {
      bindAttribute(id, i);
   }
}

ShaderProgram::ShaderProgram(ShaderProgram &&other)
   : id(other.id), shaders(std::move(other.shaders)), uniforms(std::move(other.uniforms)) {
   other.id = 0;
}

ShaderProgram::~ShaderProgram() {
   glDeleteProgram(id);
}

void ShaderProgram::attach(SPtr<Shader> shader) {
   ASSERT(shader, "Trying to attach null shader");

   glAttachShader(id, shader->getID());
   shaders.push_back(shader);
}

bool ShaderProgram::link() {
   ASSERT(shaders.size() >= 2, "Need at least two shaders to link: %lu", shaders.size());

   uniforms.clear();

   glLinkProgram(id);

   // Check the status
   GLint linkStatus;
   glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
   if (linkStatus != GL_TRUE) {
      return false;
   }

   const int nameBufSize = 512;
   char nameBuf[nameBufSize];
   GLint numUniforms;
   glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numUniforms);

   for (int i = 0; i < numUniforms; ++i) {
      GLsizei length = 0;
      GLint size = 0;
      GLenum type;
      glGetActiveUniform(id, i, nameBufSize, &length, &size, &type, nameBuf);

      if (length < 1 || size < 1) {
         LOG_WARNING("Unable to get active uniform: " << i);
      } else {
         std::string name(nameBuf);
         GLint location = glGetUniformLocation(id, name.c_str());
         uniforms.insert({name, Uniform(location, type, name)});
      }
   }

   return true;
}

void ShaderProgram::use() const {
   glUseProgram(id);
}

bool ShaderProgram::hasUniform(const std::string &name) const {
   return uniforms.count(name) > 0;
}

void ShaderProgram::commit() {
   use();

   for (UniformMap::iterator itr = uniforms.begin(); itr != uniforms.end(); ++itr) {
      itr->second.commit();
   }
}

} // namespace Shiny
