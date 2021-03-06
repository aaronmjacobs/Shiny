#ifndef SHINY_SHADER_PROGRAM_H
#define SHINY_SHADER_PROGRAM_H

#include "Shiny/Pointers.h"
#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Graphics/Uniform.h"

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
using UniformMap = std::unordered_map<std::string, UPtr<Uniform>>;

class ShaderProgram {
public:
   ShaderProgram();
   ShaderProgram(const ShaderProgram& other) = delete;
   ShaderProgram(ShaderProgram &&other);

   ~ShaderProgram();

   ShaderProgram& operator=(const ShaderProgram &other) = delete;
   ShaderProgram& operator=(ShaderProgram &&other);

   GLuint getID() const {
      return id;
   }

   void attach(const SPtr<Shader>& shader);

   bool link();

   void commit();

   bool hasUniform(const std::string &name) const {
      return uniforms.count(name) > 0;
   }

   template<typename T>
   void setUniformValue(const std::string &name, const T &value) {
      auto itr = uniforms.find(name);

      if (itr != uniforms.end()) {
         itr->second->setValue(value);
      } else {
         ASSERT(false, "Uniform with given name doesn't exist: %s", name.c_str());
      }
   }

private:
   void release();

   void move(ShaderProgram &&other);

   GLuint id;
   std::vector<SPtr<Shader>> shaders;
   UniformMap uniforms;
};

} // namespace Shiny

#endif
