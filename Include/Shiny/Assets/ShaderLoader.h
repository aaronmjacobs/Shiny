#ifndef SHINY_SHADER_LOADER_H
#define SHINY_SHADER_LOADER_H

#include "Shiny/Pointers.h"
#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Platform/Path.h"

#include <string>
#include <unordered_map>

namespace Shiny {

using ShaderDefinitions = std::unordered_map<std::string, std::string>;

struct ShaderPermutation {
   Shiny::Path path;
   ShaderDefinitions definitions;

   bool operator==(const ShaderPermutation& other) const {
      return path == other.path && definitions == other.definitions;
   }
};

} // namespace Shiny

namespace std {

template<>
struct hash<Shiny::ShaderPermutation> {
   std::size_t operator()(const Shiny::ShaderPermutation& shaderPermutation) const {
      std::size_t hashVal = hash<Shiny::Path>()(shaderPermutation.path);

      for (const auto& pair : shaderPermutation.definitions) {
         hashVal = (hashVal << 1) ^ hash<std::string>()(pair.first);
         hashVal = (hashVal >> 1) ^ hash<std::string>()(pair.second);
      }

      return hashVal;
   }
};

} // namespace std

namespace Shiny {

class Shader;
class ShaderProgram;

class ShaderLoader {
public:
   /**
   * Loads the shader with the given path and type, using a cached version if possible
   */
   SPtr<Shader> loadShader(const Path& path, const GLenum type, const std::unordered_map<std::string, std::string>& definitions = {});

   /**
   * Loads the shader program comprised of shaders with the given path (and their respective extensions),
   * using a cached version if possible
   */
   SPtr<ShaderProgram> loadShaderProgram(const Path& path, const std::unordered_map<std::string, std::string>& definitions = {});

   /**
   * Reloads all mapped shaders from source
   */
   void reloadShaders();

private:
   std::unordered_map<ShaderPermutation, SPtr<Shader>> shaderMap;
   std::unordered_map<ShaderPermutation, SPtr<ShaderProgram>> shaderProgramMap;

   SPtr<Shader> defaultVertexShader;
   SPtr<Shader> defaultGeometryShader;
   SPtr<Shader> defaultFragmentShader;
   SPtr<ShaderProgram> defaultShaderProgram;

   SPtr<Shader> getDefaultShader(const GLenum type);
   SPtr<ShaderProgram> getDefaultShaderProgram();
};

} // namespace Shiny

#endif
