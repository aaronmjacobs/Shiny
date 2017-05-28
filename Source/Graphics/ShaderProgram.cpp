#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/Shader.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Graphics/UniformTypes.h"

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

namespace Shiny {

namespace {

UPtr<Uniform> createUniform(const std::string& uniformName, const GLint uniformLocation, const GLenum uniformType, const GLuint program) {
   switch (uniformType) {
      case GL_FLOAT:
         return std::make_unique<FloatUniform>(uniformName, uniformLocation, uniformType, program);
      case GL_INT:
      case GL_SAMPLER_1D:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_3D:
      case GL_SAMPLER_CUBE:
      case GL_SAMPLER_1D_SHADOW:
      case GL_SAMPLER_2D_SHADOW:
      case GL_SAMPLER_1D_ARRAY:
      case GL_SAMPLER_2D_ARRAY:
      case GL_SAMPLER_1D_ARRAY_SHADOW:
      case GL_SAMPLER_2D_ARRAY_SHADOW:
      case GL_SAMPLER_2D_MULTISAMPLE:
      case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_SAMPLER_CUBE_SHADOW:
      case GL_SAMPLER_BUFFER:
      case GL_SAMPLER_2D_RECT:
      case GL_SAMPLER_2D_RECT_SHADOW:
      case GL_INT_SAMPLER_1D:
      case GL_INT_SAMPLER_2D:
      case GL_INT_SAMPLER_3D:
      case GL_INT_SAMPLER_CUBE:
      case GL_INT_SAMPLER_1D_ARRAY:
      case GL_INT_SAMPLER_2D_ARRAY:
      case GL_INT_SAMPLER_2D_MULTISAMPLE:
      case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_INT_SAMPLER_BUFFER:
      case GL_INT_SAMPLER_2D_RECT:
      case GL_UNSIGNED_INT_SAMPLER_1D:
      case GL_UNSIGNED_INT_SAMPLER_2D:
      case GL_UNSIGNED_INT_SAMPLER_3D:
      case GL_UNSIGNED_INT_SAMPLER_CUBE:
      case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
      case GL_UNSIGNED_INT_SAMPLER_BUFFER:
      case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
         return std::make_unique<IntUniform>(uniformName, uniformLocation, uniformType, program);
      case GL_UNSIGNED_INT:
         return std::make_unique<UintUniform>(uniformName, uniformLocation, uniformType, program);
      case GL_BOOL:
         return std::make_unique<BoolUniform>(uniformName, uniformLocation, uniformType, program);

      case GL_FLOAT_VEC2:
         return std::make_unique<Float2Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_VEC3:
         return std::make_unique<Float3Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_VEC4:
         return std::make_unique<Float4Uniform>(uniformName, uniformLocation, uniformType, program);

      case GL_INT_VEC2:
         return std::make_unique<Int2Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_INT_VEC3:
         return std::make_unique<Int3Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_INT_VEC4:
         return std::make_unique<Int4Uniform>(uniformName, uniformLocation, uniformType, program);

      case GL_UNSIGNED_INT_VEC2:
         return std::make_unique<Uint2Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_UNSIGNED_INT_VEC3:
         return std::make_unique<Uint3Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_UNSIGNED_INT_VEC4:
         return std::make_unique<Uint4Uniform>(uniformName, uniformLocation, uniformType, program);

      case GL_BOOL_VEC2:
         return std::make_unique<Bool2Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_BOOL_VEC3:
         return std::make_unique<Bool3Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_BOOL_VEC4:
         return std::make_unique<Bool4Uniform>(uniformName, uniformLocation, uniformType, program);

      case GL_FLOAT_MAT2:
         return std::make_unique<Float2x2Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT2x3:
         return std::make_unique<Float2x3Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT2x4:
         return std::make_unique<Float2x4Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT3x2:
         return std::make_unique<Float3x2Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT3:
         return std::make_unique<Float3x3Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT3x4:
         return std::make_unique<Float3x4Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT4x2:
         return std::make_unique<Float4x2Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT4x3:
         return std::make_unique<Float4x3Uniform>(uniformName, uniformLocation, uniformType, program);
      case GL_FLOAT_MAT4:
         return std::make_unique<Float4x4Uniform>(uniformName, uniformLocation, uniformType, program);

      default:
         ASSERT(false, "Invalid uniform type: %u", uniformType);
         return nullptr;
   }
}

void bindAttribute(GLuint id, GLint attribute) {
   glBindAttribLocation(id, attribute, ShaderAttributes::kNames[attribute]);
}

} // namespace

ShaderProgram::ShaderProgram()
   : id(glCreateProgram()) {
   for (size_t i = 0; i < ShaderAttributes::kNames.size(); ++i) {
      bindAttribute(id, static_cast<GLint>(i));
   }
}

ShaderProgram::ShaderProgram(ShaderProgram &&other) {
   move(std::move(other));
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram &&other) {
   release();
   move(std::move(other));
   return *this;
}

ShaderProgram::~ShaderProgram() {
   release();
}

void ShaderProgram::release() {
   glDeleteProgram(id);
   id = 0;
}

void ShaderProgram::move(ShaderProgram &&other) {
   id = other.id;
   shaders = std::move(other.shaders);
   uniforms = std::move(other.uniforms);

   other.id = 0;
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

         uniforms.emplace(name, createUniform(name, location, type, id));
      }
   }

   return true;
}

void ShaderProgram::use() const {
   Context::current()->useProgram(id);
}

bool ShaderProgram::hasUniform(const std::string &name) const {
   return uniforms.count(name) > 0;
}

void ShaderProgram::commit() {
   use();

   for (const auto& pair : uniforms) {
      pair.second->commit();
   }
}

} // namespace Shiny
