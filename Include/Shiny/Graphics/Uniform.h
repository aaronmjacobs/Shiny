#ifndef SHINY_UNIFORM_H
#define SHINY_UNIFORM_H

#include "Shiny/ShinyAssert.h"
#include "Shiny/Graphics/OpenGL.h"

#include <glm/glm.hpp>

#include <string>

namespace Shiny {

class Uniform {
public:
   Uniform(const std::string& uniformName, const GLint uniformLocation, const GLenum uniformType, const GLuint program)
      : name(uniformName), location(uniformLocation), type(uniformType), dirty(false) {
      ASSERT(program != 0);
   }

   Uniform(const Uniform& other) = delete;
   Uniform(Uniform&& other) = delete;

   virtual ~Uniform() = default;

   Uniform& operator=(const Uniform& other) = delete;
   Uniform& operator=(Uniform&& other) = delete;

   const std::string& getName() const {
      return name;
   }

   GLint getLocation() const {
      return location;
   }

   GLenum getType() const {
      return type;
   }

   void commit() {
      if (dirty) {
         commitData();
         dirty = false;
      }
   }

   template<typename T>
   void setValue(const T& value) {
      bool valueChanged = setData(value);
      dirty = dirty || valueChanged;
   }

protected:
   virtual void commitData() = 0;

#define DECLARE_SET_DATA(type) virtual bool setData(type value) { return typeError(#type); }

   DECLARE_SET_DATA(GLfloat)
   DECLARE_SET_DATA(GLint)
   DECLARE_SET_DATA(GLuint)

   DECLARE_SET_DATA(glm::fvec2)
   DECLARE_SET_DATA(glm::fvec3)
   DECLARE_SET_DATA(glm::fvec4)
   
   DECLARE_SET_DATA(glm::ivec2)
   DECLARE_SET_DATA(glm::ivec3)
   DECLARE_SET_DATA(glm::ivec4)

   DECLARE_SET_DATA(glm::uvec2)
   DECLARE_SET_DATA(glm::uvec3)
   DECLARE_SET_DATA(glm::uvec4)

   // OpenGL treats booleans as integers when setting / getting uniform values, so we provide wrapper functions
   bool setData(bool value) {
      return setData(static_cast<GLint>(value));
   }
   bool setData(glm::bvec2 value) {
      return setData(glm::ivec2(value.x, value.y));
   }
   bool setData(glm::bvec3 value) {
      return setData(glm::ivec3(value.x, value.y, value.z));
   }
   bool setData(glm::bvec4 value) {
      return setData(glm::ivec4(value.x, value.y, value.z, value.w));
   }

#undef DECLARE_SET_DATA
#define DECLARE_SET_DATA_REF(type) virtual bool setData(const type& value) { return typeError(#type); }

   DECLARE_SET_DATA_REF(glm::fmat2x2)
   DECLARE_SET_DATA_REF(glm::fmat2x3)
   DECLARE_SET_DATA_REF(glm::fmat2x4)
   DECLARE_SET_DATA_REF(glm::fmat3x2)
   DECLARE_SET_DATA_REF(glm::fmat3x3)
   DECLARE_SET_DATA_REF(glm::fmat3x4)
   DECLARE_SET_DATA_REF(glm::fmat4x2)
   DECLARE_SET_DATA_REF(glm::fmat4x3)
   DECLARE_SET_DATA_REF(glm::fmat4x4)

#undef DECLARE_SET_DATA_REF

   virtual const char* getTypeName() const = 0;

private:
   bool typeError(const char* typeName) {
      ASSERT(false, "Trying to set uniform with invalid type (%s, should be %s): %s", typeName, getTypeName(), name.c_str());
      return false;
   }

   const std::string name;
   const GLint location;
   const GLenum type;
   bool dirty;
};

} // namespace Shiny

#endif
