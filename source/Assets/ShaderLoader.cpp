#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Assets/ShaderLoader.h"

#include "Shiny/Graphics/Shader.h"
#include "Shiny/Graphics/ShaderProgram.h"

#include "Shiny/Platform/IOUtils.h"

#define GLSL(source) "#version 150 core\n" #source

namespace Shiny {

namespace {

const char* VERTEX_EXTENSION = ".vert";
const char* GEOMETRY_EXTENSION = ".geom";
const char* FRAGMENT_EXTENSION = ".frag";

const char* DEFAULT_VERTEX_SOURCE = GLSL(
   uniform mat4 uModelMatrix;
   uniform mat4 uViewMatrix;
   uniform mat4 uProjMatrix;

   in vec3 aPosition;

   void main() {
      gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
   }
);

const char* DEFAULT_GEOMETRY_SOURCE = GLSL(
   layout(points) in;
   layout(points, max_vertices = 1) out;

   void main() {
      gl_Position = gl_in[0].gl_Position;
      EmitVertex();
      EndPrimitive();
   }
);

const char* DEFAULT_FRAGMENT_SOURCE = GLSL(
   out vec4 color;

   void main() {
      color = vec4(1.0, 0.35, 0.44, 1.0);
   }
);

const char* getShaderTypeName(const GLenum type) {
   switch (type) {
      case GL_VERTEX_SHADER:
         return "vertex";
      case GL_GEOMETRY_SHADER:
         return "geometry";
      case GL_FRAGMENT_SHADER:
         return "fragment";
      default:
         return "unknown";
   }
}

std::string getShaderError(GLuint id, bool isProgram) {
   GLint infoLogLength;

   if (isProgram) {
      glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
   } else {
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
   }

   if (infoLogLength < 1) {
      return std::string();
   }

   UPtr<GLchar[]> strInfoLog(new GLchar[infoLogLength]);

   if (isProgram) {
      glGetProgramInfoLog(id, infoLogLength, NULL, strInfoLog.get());
   } else {
      glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog.get());
   }

   // If the log ends in a newline, nuke it
   if (infoLogLength >= 2 && strInfoLog[infoLogLength - 2] == '\n') {
      strInfoLog[infoLogLength - 2] = '\0';
   }

   return std::string(strInfoLog.get());
}

std::string getShaderCompileError(const SPtr<Shader> &shader) {
   return getShaderError(shader->getID(), false);
}

std::string getShaderLinkError(const SPtr<ShaderProgram> &shaderProgram) {
   return getShaderError(shaderProgram->getID(), true);
}

const char* getDefaultShaderSource(const GLenum type) {
   switch (type) {
      case GL_VERTEX_SHADER:
         return DEFAULT_VERTEX_SOURCE;
      case GL_GEOMETRY_SHADER:
         return DEFAULT_GEOMETRY_SOURCE;
      case GL_FRAGMENT_SHADER:
         return DEFAULT_FRAGMENT_SOURCE;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
         return DEFAULT_VERTEX_SOURCE;
   }
}

SPtr<Shader> getDefaultShader(const GLenum type) {
   static SPtr<Shader> defaultVertexShader = nullptr;
   static SPtr<Shader> defaultGeometryShader = nullptr;
   static SPtr<Shader> defaultFragmentShader = nullptr;

   switch (type) {
      case GL_VERTEX_SHADER:
         if (defaultVertexShader) {
            return defaultVertexShader;
         }
         break;
      case GL_GEOMETRY_SHADER:
         if (defaultGeometryShader) {
            return defaultGeometryShader;
         }
         break;
      case GL_FRAGMENT_SHADER:
         if (defaultFragmentShader) {
            return defaultFragmentShader;
         }
         break;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
   }

   SPtr<Shader> shader(std::make_shared<Shader>(type));
   if (!shader->compile(getDefaultShaderSource(type))) {
      LOG_MESSAGE("Error compiling default " << getShaderTypeName(type) << " shader. Error message: \""
                  << getShaderCompileError(shader) << "\"");
      LOG_FATAL("Unable to compile default " << getShaderTypeName(type) << " shader");
   }

   switch (type) {
      case GL_VERTEX_SHADER:
         defaultVertexShader = shader;
         break;
      case GL_GEOMETRY_SHADER:
         defaultGeometryShader = shader;
         break;
      case GL_FRAGMENT_SHADER:
         defaultFragmentShader = shader;
         break;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
         return nullptr;
   }

   return shader;
}

SPtr<ShaderProgram> getDefaultShaderProgram() {
   static SPtr<ShaderProgram> defaultShaderProgram = nullptr;

   if (defaultShaderProgram) {
      return defaultShaderProgram;
   }

   defaultShaderProgram = std::make_shared<ShaderProgram>();
   defaultShaderProgram->attach(getDefaultShader(GL_VERTEX_SHADER));
   defaultShaderProgram->attach(getDefaultShader(GL_FRAGMENT_SHADER));

   if (!defaultShaderProgram->link()) {
      LOG_MESSAGE("Error linking default shader program. Error message: \""
                  << getShaderLinkError(defaultShaderProgram) << "\"");
      LOG_FATAL("Unable to link default shader");
   }

   return defaultShaderProgram;
}

} // namespace

ShaderLoader::ShaderLoader() {
}

ShaderLoader::~ShaderLoader() {
}

SPtr<Shader> ShaderLoader::loadShader(const std::string &fileName, const GLenum type) {
   ASSERT(type == GL_VERTEX_SHADER || type == GL_GEOMETRY_SHADER || type == GL_FRAGMENT_SHADER,
          "Invalid shader type: %i", type);

   ShaderMap::iterator location(shaderMap.find(fileName));
   if (location != shaderMap.end()) {
      return location->second;
   }

   std::string source;
   if (!IOUtils::readFromFile(fileName, source)) {
      LOG_WARNING("Unable to load shader from file \"" << fileName << "\", reverting to default shader");
      return getDefaultShader(type);
   }

   SPtr<Shader> shader(std::make_shared<Shader>(type));
   if (!shader->compile(source.c_str())) {
      LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \""
                  << fileName << "\", reverting to default shader. Error message: \""
                  << getShaderCompileError(shader) << "\"");
      shader = getDefaultShader(type);
   }

   shaderMap.insert({ fileName, shader });
   return shader;
}

SPtr<ShaderProgram> ShaderLoader::loadShaderProgram(const std::string &fileName) {
   ShaderProgramMap::iterator location(shaderProgramMap.find(fileName));
   if (location != shaderProgramMap.end()) {
      return location->second;
   }

   SPtr<ShaderProgram> shaderProgram(std::make_shared<ShaderProgram>());
   std::string vertexFileName = fileName + VERTEX_EXTENSION;
   std::string geometryFileName = fileName + GEOMETRY_EXTENSION;
   std::string fragmentFileName = fileName + FRAGMENT_EXTENSION;

   if (IOUtils::canRead(vertexFileName)) {
      shaderProgram->attach(loadShader(vertexFileName, GL_VERTEX_SHADER));
   }

   if (IOUtils::canRead(geometryFileName)) {
      shaderProgram->attach(loadShader(geometryFileName, GL_GEOMETRY_SHADER));
   }

   if (IOUtils::canRead(fragmentFileName)) {
      shaderProgram->attach(loadShader(fragmentFileName, GL_FRAGMENT_SHADER));
   }

   if (!shaderProgram->link()) {
      LOG_WARNING("Unable to link '" << fileName
                  << "' shader program, reverting to default shader program. Error message: \""
                  << getShaderLinkError(shaderProgram) << "\"");
      shaderProgram = getDefaultShaderProgram();
   }

   shaderProgramMap.insert({ fileName, shaderProgram });
   return shaderProgram;
}

void ShaderLoader::reloadShaders() {
   // TODO Only reload if files have been updated (check file modification time)

   for (ShaderMap::iterator itr = shaderMap.begin(); itr != shaderMap.end(); ++itr) {
      const std::string& fileName = itr->first;
      const SPtr<Shader> &shader = itr->second;

      std::string source;
      if (!IOUtils::readFromFile(fileName, source)) {
         LOG_WARNING("Unable to load shader from file \"" << fileName << "\", not reloading");
         continue;
      }

      if (!shader->compile(source.c_str())) {
         LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \""
                     << fileName << "\", reverting to default shader. Error message: \""
                     << getShaderCompileError(shader) << "\"");

         const std::string &defaultSource = getDefaultShaderSource(shader->getType());
         if (!shader->compile(defaultSource.c_str())) {
            LOG_MESSAGE("Error compiling default " << getShaderTypeName(shader->getType())
                        << " shader. Error message: \"" << getShaderCompileError(shader) << "\"");
            LOG_FATAL("Unable to compile default " << getShaderTypeName(shader->getType()) << " shader");
         }
      }
   }

   for (ShaderProgramMap::iterator itr = shaderProgramMap.begin(); itr != shaderProgramMap.end(); ++itr) {
      const std::string& fileName = itr->first;
      const SPtr<ShaderProgram> &shaderProgram = itr->second;

      if (!shaderProgram->link()) {
         LOG_WARNING("Unable to link '" << fileName << "' shader program. Error message: \""
                     << getShaderLinkError(shaderProgram) << "\"");
      }
   }
}

} // namespace Shiny
