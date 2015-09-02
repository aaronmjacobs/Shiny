#ifndef SHINY_SHADER_PROGRAM_H
#define SHINY_SHADER_PROGRAM_H

#include "Shiny/Defines.h"
#include "Shiny/GLIncludes.h"
#include "Shiny/Log.h"
#include "Shiny/Pointers.h"

#include <glm/glm.hpp>

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace Shiny {

namespace ShaderAttributes {

enum SHINYAPI Attributes : GLint {
   POSITION = 0,
   NORMAL = 1,
   TEX_COORD = 2
};

namespace {

SHINYAPI const std::array<const char*, 3> NAMES = {{
   "aPosition",
   "aNormal",
   "aTexCoord"
}};

} // namespace

} // namespace ShaderAttributes

union SHINYAPI UniformData {
   bool boolVal;
   int intVal;
   float floatVal;
   glm::vec2 vec2Val;
   glm::vec3 vec3Val;
   glm::vec4 vec4Val;
   glm::mat4 mat4Val;
};

class SHINYAPI Uniform {
protected:
   UniformData data;
   const std::string name;
   const GLint location;
   const GLenum type;
   bool dirty;

public:
   Uniform(const GLint location, const GLenum type, const std::string &name);

   void poll(const GLuint program);

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

   void setValue(const glm::vec2 &value);

   void setValue(const glm::vec3 &value);

   void setValue(const glm::vec4 &value);

   void setValue(const glm::mat4 &value);
};

class Shader;
typedef std::unordered_map<std::string, Uniform> UniformMap;

class SHINYAPI ShaderProgram {
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
