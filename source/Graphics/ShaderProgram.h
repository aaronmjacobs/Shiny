#ifndef SHINY_SHADER_PROGRAM_H
#define SHINY_SHADER_PROGRAM_H

#include "Constants.h"
#include "GLIncludes.h"
#include "Log.h"
#include "Pointers.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace Shiny {

#ifdef SHINY_USE_GLM

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
#define SHINY_RAW_VALUE(x) glm::value_ptr(x)

#else // SHINY_USE_GLM

typedef std::array<float, 2> vec2;
typedef std::array<float, 3> vec3;
typedef std::array<float, 4> vec4;
typedef std::array<float, 16> mat4;
#define SHINY_RAW_VALUE(x) (x).data()

#endif // SHINY_USE_GLM

namespace ShaderAttributes {

enum Attributes : GLint {
   POSITION = 0,
   NORMAL = 1,
   TEX_COORD = 2
};

namespace {

const std::array<const char*, 3> NAMES = {{
   "aPosition",
   "aNormal",
   "aTexCoord"
}};

} // namespace

} // namespace ShaderAttributes

union UniformData {
   bool boolVal;
   int intVal;
   float floatVal;
   vec2 vec2Val;
   vec3 vec3Val;
   vec4 vec4Val;
   mat4 mat4Val;
};

class Uniform {
protected:
   UniformData data;
   const std::string name;
   const GLint location;
   const GLenum type;
   bool dirty;

public:
   Uniform(const GLint location, const GLenum type, const std::string &name);

   void commit();

   GLenum getType() const {
      return type;
   }

   const std::string& getName() const {
      return name;
   }

   void setValue(bool value);

   void setValue(int value);

   void setValue(GLenum value);

   void setValue(float value);

   void setValue(const vec2 &value);

   void setValue(const vec3 &value);

   void setValue(const vec4 &value);

   void setValue(const mat4 &value);
};

class Shader;
typedef std::unordered_map<std::string, Uniform> UniformMap;

class ShaderProgram {
protected:
   GLuint id;
   std::vector<SPtr<Shader>> shaders;
   UniformMap uniforms;

   void use() const;

public:
   ShaderProgram();

   ShaderProgram(ShaderProgram &&other);

   virtual ~ShaderProgram();

   GLuint getID() const {
      return id;
   }

   void attach(SPtr<Shader> shader);

   bool link();

   bool hasUniform(const std::string &name) const;

   void commit();

   template<typename T>
   void setUniformValue(const std::string &name, const T &value, bool ignoreFailure = false) {
      UniformMap::iterator itr = uniforms.find(name);
      if (itr == uniforms.end()) {
         if (!ignoreFailure) {
            LOG_WARNING("Uniform with given name doesn't exist: " << name);
         }

         return;
      }

      itr->second.setValue(value);
   }
};

} // namespace Shiny

#endif
