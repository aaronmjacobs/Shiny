#ifndef SHINY_SHADER_PROGRAM_H
#define SHINY_SHADER_PROGRAM_H

#include "Shiny/Log.h"
#include "Shiny/Pointers.h"

#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Graphics/Uniform.h"

#include <glm/glm.hpp>

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace Shiny {

namespace ShaderAttributes {

enum Attributes : GLint {
   kPosition = 0,
   kNormal = 1,
   kTexCoord = 2
};

namespace {

const std::array<const char*, 3> kNames = {{
   "aPosition",
   "aNormal",
   "aTexCoord"
}};

} // namespace

} // namespace ShaderAttributes

class Shader;

class ShaderProgram {
protected:
   GLuint id;
   std::vector<SPtr<Shader>> shaders;
   std::unordered_map<std::string, UPtr<Uniform>> uniforms;

   void release();

   void move(ShaderProgram &&other);

   void use() const;

public:
   ShaderProgram();

   ShaderProgram(ShaderProgram &&other);

   ShaderProgram& operator=(ShaderProgram &&other);

   virtual ~ShaderProgram();

   GLuint getID() const {
      return id;
   }

   void attach(SPtr<Shader> shader);

   bool link();

   bool hasUniform(const std::string &name) const;

   void commit();

   template<typename T>
   void setUniformValue(const std::string &name, const T &value) {
      auto itr = uniforms.find(name);

      if (itr != uniforms.end()) {
         itr->second->setValue(value);
      } else {
         ASSERT(false, "Uniform with given name doesn't exist: %s", name.c_str());
      }
   }
};

} // namespace Shiny

#endif
