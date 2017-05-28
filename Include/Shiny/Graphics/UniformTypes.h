#ifndef SHINY_UNIFORM_TYPES_H
#define SHINY_UNIFORM_TYPES_H

#include "Shiny/Graphics/Uniform.h"

#include <glm/gtc/type_ptr.hpp>

namespace Shiny {

namespace {

bool typesMatch(const GLenum uniformType) {
   return false;
}

template<typename First, typename... Rest>
bool typesMatch(const GLenum uniformType, First first, Rest... rest) {
   return uniformType == first || typesMatch(uniformType, rest...);
}

template<typename... Args>
void checkTypeEnum(const GLenum uniformType, Args... args) {
   ASSERT(typesMatch(uniformType, args...), "Invalid uniform type: %u", uniformType);
}

} // namespace

#define DECLARE_UNIFORM_TYPE(class_name, data_type, param_type, get_function, set_function, ...)\
class class_name : public Uniform {\
public:\
   class_name(const std::string& uniformName, const GLint uniformLocation, const GLenum uniformType, const GLuint program)\
      : Uniform(uniformName, uniformLocation, uniformType, program), data{} {\
      checkTypeEnum(uniformType, __VA_ARGS__);\
      get_function;\
   }\
\
protected:\
   virtual void commitData() override {\
      set_function;\
   }\
\
   virtual bool setData(param_type value) override {\
      bool valueChanged = data != value;\
      data = value;\
      return valueChanged;\
   }\
\
   virtual const char* getTypeName() const override {\
      return #data_type;\
   }\
\
private:\
   data_type data;\
};

#define DECLARE_UNIFORM_TYPE_BASIC(class_name, data_type, get_function_name, set_function_name, ...)\
        DECLARE_UNIFORM_TYPE(class_name, data_type, data_type, get_function_name(program, uniformLocation, &data), set_function_name(getLocation(), data), __VA_ARGS__)

#define VALID_INT_ENUMS \
GL_INT, GL_SAMPLER_1D, GL_SAMPLER_2D, GL_SAMPLER_3D, GL_SAMPLER_CUBE, GL_SAMPLER_1D_SHADOW, GL_SAMPLER_2D_SHADOW,\
GL_SAMPLER_1D_ARRAY, GL_SAMPLER_2D_ARRAY, GL_SAMPLER_1D_ARRAY_SHADOW, GL_SAMPLER_2D_ARRAY_SHADOW,\
GL_SAMPLER_2D_MULTISAMPLE, GL_SAMPLER_2D_MULTISAMPLE_ARRAY, GL_SAMPLER_CUBE_SHADOW, GL_SAMPLER_BUFFER,\
GL_SAMPLER_2D_RECT, GL_SAMPLER_2D_RECT_SHADOW, GL_INT_SAMPLER_1D, GL_INT_SAMPLER_2D, GL_INT_SAMPLER_3D,\
GL_INT_SAMPLER_CUBE, GL_INT_SAMPLER_1D_ARRAY, GL_INT_SAMPLER_2D_ARRAY, GL_INT_SAMPLER_2D_MULTISAMPLE,\
GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, GL_INT_SAMPLER_BUFFER, GL_INT_SAMPLER_2D_RECT, GL_UNSIGNED_INT_SAMPLER_1D,\
GL_UNSIGNED_INT_SAMPLER_2D, GL_UNSIGNED_INT_SAMPLER_3D, GL_UNSIGNED_INT_SAMPLER_CUBE, GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,\
GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,\
GL_UNSIGNED_INT_SAMPLER_BUFFER, GL_UNSIGNED_INT_SAMPLER_2D_RECT

DECLARE_UNIFORM_TYPE_BASIC(FloatUniform, GLfloat, glGetUniformfv, glUniform1f, GL_FLOAT)
DECLARE_UNIFORM_TYPE_BASIC(IntUniform, GLint, glGetUniformiv, glUniform1i, VALID_INT_ENUMS)
DECLARE_UNIFORM_TYPE_BASIC(UintUniform, GLuint, glGetUniformuiv, glUniform1ui, GL_UNSIGNED_INT)
DECLARE_UNIFORM_TYPE_BASIC(BoolUniform, GLint, glGetUniformiv, glUniform1i, GL_BOOL)

#undef VALID_INT_ENUMS
#undef DECLARE_UNIFORM_TYPE_BASIC

#define DECLARE_UNIFORM_TYPE_VECTOR(class_name, data_type, get_function_name, set_function_name, ...)\
        DECLARE_UNIFORM_TYPE(class_name, data_type, data_type, get_function_name(program, uniformLocation, glm::value_ptr(data)), set_function_name(getLocation(), 1, glm::value_ptr(data)), __VA_ARGS__)

DECLARE_UNIFORM_TYPE_VECTOR(Float2Uniform, glm::fvec2, glGetUniformfv, glUniform2fv, GL_FLOAT_VEC2)
DECLARE_UNIFORM_TYPE_VECTOR(Float3Uniform, glm::fvec3, glGetUniformfv, glUniform3fv, GL_FLOAT_VEC3)
DECLARE_UNIFORM_TYPE_VECTOR(Float4Uniform, glm::fvec4, glGetUniformfv, glUniform4fv, GL_FLOAT_VEC4)

DECLARE_UNIFORM_TYPE_VECTOR(Int2Uniform, glm::ivec2, glGetUniformiv, glUniform2iv, GL_INT_VEC2)
DECLARE_UNIFORM_TYPE_VECTOR(Int3Uniform, glm::ivec3, glGetUniformiv, glUniform3iv, GL_INT_VEC3)
DECLARE_UNIFORM_TYPE_VECTOR(Int4Uniform, glm::ivec4, glGetUniformiv, glUniform4iv, GL_INT_VEC4)

DECLARE_UNIFORM_TYPE_VECTOR(Uint2Uniform, glm::uvec2, glGetUniformuiv, glUniform2uiv, GL_UNSIGNED_INT_VEC2)
DECLARE_UNIFORM_TYPE_VECTOR(Uint3Uniform, glm::uvec3, glGetUniformuiv, glUniform3uiv, GL_UNSIGNED_INT_VEC3)
DECLARE_UNIFORM_TYPE_VECTOR(Uint4Uniform, glm::uvec4, glGetUniformuiv, glUniform4uiv, GL_UNSIGNED_INT_VEC4)

DECLARE_UNIFORM_TYPE_VECTOR(Bool2Uniform, glm::ivec2, glGetUniformiv, glUniform2iv, GL_BOOL_VEC2)
DECLARE_UNIFORM_TYPE_VECTOR(Bool3Uniform, glm::ivec3, glGetUniformiv, glUniform3iv, GL_BOOL_VEC3)
DECLARE_UNIFORM_TYPE_VECTOR(Bool4Uniform, glm::ivec4, glGetUniformiv, glUniform4iv, GL_BOOL_VEC4)

#undef DECLARE_UNIFORM_TYPE_VECTOR

#define DECLARE_UNIFORM_TYPE_MATRIX(class_name, data_type, get_function_name, set_function_name, ...)\
        DECLARE_UNIFORM_TYPE(class_name, data_type, const data_type&, get_function_name(program, uniformLocation, glm::value_ptr(data)), set_function_name(getLocation(), 1, GL_FALSE, glm::value_ptr(data)), __VA_ARGS__)

DECLARE_UNIFORM_TYPE_MATRIX(Float2x2Uniform, glm::fmat2x2, glGetUniformfv, glUniformMatrix2fv, GL_FLOAT_MAT2)
DECLARE_UNIFORM_TYPE_MATRIX(Float2x3Uniform, glm::fmat2x3, glGetUniformfv, glUniformMatrix2x3fv, GL_FLOAT_MAT2x3)
DECLARE_UNIFORM_TYPE_MATRIX(Float2x4Uniform, glm::fmat2x4, glGetUniformfv, glUniformMatrix2x4fv, GL_FLOAT_MAT2x4)
DECLARE_UNIFORM_TYPE_MATRIX(Float3x2Uniform, glm::fmat3x2, glGetUniformfv, glUniformMatrix3x2fv, GL_FLOAT_MAT3x2)
DECLARE_UNIFORM_TYPE_MATRIX(Float3x3Uniform, glm::fmat3x3, glGetUniformfv, glUniformMatrix3fv, GL_FLOAT_MAT3)
DECLARE_UNIFORM_TYPE_MATRIX(Float3x4Uniform, glm::fmat3x4, glGetUniformfv, glUniformMatrix3x4fv, GL_FLOAT_MAT3x4)
DECLARE_UNIFORM_TYPE_MATRIX(Float4x2Uniform, glm::fmat4x2, glGetUniformfv, glUniformMatrix4x2fv, GL_FLOAT_MAT4x2)
DECLARE_UNIFORM_TYPE_MATRIX(Float4x3Uniform, glm::fmat4x3, glGetUniformfv, glUniformMatrix4x3fv, GL_FLOAT_MAT4x3)
DECLARE_UNIFORM_TYPE_MATRIX(Float4x4Uniform, glm::fmat4x4, glGetUniformfv, glUniformMatrix4fv, GL_FLOAT_MAT4)

#undef DECLARE_UNIFORM_TYPE_MATRIX

#undef DECLARE_UNIFORM_TYPE

} // namespace Shiny

#endif
