#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/Shader.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Graphics/UniformTypes.h"

#include <cstring>
#include <sstream>

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

std::vector<std::string> getUniformNames(const char* nameBuf, GLsizei length, GLint size) {
   ASSERT(size > 0);

   if (size == 1) {
      // Not an array, return as is
      return { nameBuf };
   }

   ASSERT(length > 3 && nameBuf[length - 3] == '[' && nameBuf[length - 2] == '0' && nameBuf[length - 1] == ']');
   std::string baseName(nameBuf, length - 3);

   std::vector<std::string> names(size);
   for (GLint i = 0; i < size; ++i) {
      std::stringstream ss;
      ss << baseName << "[" << i << "]";
      names[i] = ss.str();
   }

   return names;
}

void createProgramUniformsAtIndex(UniformMap& uniformMap, GLuint program, GLuint index) {
   std::array<GLchar, 256> nameBuf;
   GLsizei length = 0;
   GLint size = 0;
   GLenum type = GL_FLOAT;
   glGetActiveUniform(program, index, static_cast<GLsizei>(nameBuf.size()), &length, &size, &type, nameBuf.data());

   if (length < 1) {
      LOG_WARNING("Unable to get active uniform " << index << " for program " << program);
      return;
   }

   std::vector<std::string> uniformNames = getUniformNames(nameBuf.data(), length, size);
   for (const std::string& uniformName : uniformNames) {
      GLint location = glGetUniformLocation(program, uniformName.c_str());
      ASSERT(location >= 0);

      uniformMap.emplace(uniformName, createUniform(uniformName, location, type, program));
   }
}

void createProgramUniforms(UniformMap& uniformMap, GLuint program) {
   GLint numUniforms = 0;
   glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

   for (GLint i = 0; i < numUniforms; ++i) {
      createProgramUniformsAtIndex(uniformMap, program, i);
   }
}

void bindAttribute(GLuint program, GLuint attribute) {
   glBindAttribLocation(program, attribute, ShaderAttributes::kNames[attribute]);
}

} // namespace

ShaderProgram::ShaderProgram()
   : id(glCreateProgram()) {
   for (size_t i = 0; i < ShaderAttributes::kNames.size(); ++i) {
      bindAttribute(id, static_cast<GLuint>(i));
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

void ShaderProgram::attach(const SPtr<Shader>& shader) {
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

   createProgramUniforms(uniforms, id);

   return true;
}

void ShaderProgram::commit() {
   Context::current()->useProgram(id);

   for (const auto& pair : uniforms) {
      pair.second->commit();
   }
}

} // namespace Shiny
