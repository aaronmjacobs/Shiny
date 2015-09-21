#ifndef SHINY_SHADER_LOADER_H
#define SHINY_SHADER_LOADER_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include "Shiny/Graphics/OpenGL.h"

#include <string>
#include <unordered_map>

namespace Shiny {

class Shader;
class ShaderProgram;

typedef std::unordered_map<std::string, SPtr<Shader>> ShaderMap;
typedef std::unordered_map<std::string, SPtr<ShaderProgram>> ShaderProgramMap;

class SHINYAPI ShaderLoader {
protected:
   ShaderMap shaderMap;
   ShaderProgramMap shaderProgramMap;

public:
   ShaderLoader();

   virtual ~ShaderLoader();

   /**
    * Loads the shader with the given file name and type, using a cached version if possible
    */
   SPtr<Shader> loadShader(const std::string &fileName, const GLenum type);

   /**
    * Loads the shader program comprised of shaders with the given name (and their respective extensions),
    * using a cached version if possible
    */
   SPtr<ShaderProgram> loadShaderProgram(const std::string &name);

   /**
    * Reloads all mapped shaders from source
    */
   void reloadShaders();
};

} // namespace Shiny

#endif
