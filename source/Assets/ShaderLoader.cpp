#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Assets/ShaderLoader.h"

#include "Shiny/Graphics/Shader.h"
#include "Shiny/Graphics/ShaderProgram.h"

#include "Shiny/Platform/IOUtils.h"

#if defined(GLSL)
#  undef GLSL
#endif
#define GLSL(source) "#version 150 core\n" #source

namespace Shiny {

namespace {

const char* kVertexExtension = ".vert";
const char* kGeometryExtension = ".geom";
const char* kFragmentExtension = ".frag";

const char* kDefaultVertexSource = GLSL(
   uniform mat4 uModelMatrix;
   uniform mat4 uViewMatrix;
   uniform mat4 uProjMatrix;

   in vec3 aPosition;

   void main() {
      gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
   }
);

const char* kDefaultGeometrySource = GLSL(
   layout(points) in;
   layout(points, max_vertices = 1) out;

   void main() {
      gl_Position = gl_in[0].gl_Position;
      EmitVertex();
      EndPrimitive();
   }
);

const char* kDefaultFragmentSource = GLSL(
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
         return kDefaultVertexSource;
      case GL_GEOMETRY_SHADER:
         return kDefaultGeometrySource;
      case GL_FRAGMENT_SHADER:
         return kDefaultFragmentSource;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
         return kDefaultVertexSource;
   }
}

SPtr<Shader> createDefaultShader(const GLenum type) {
   SPtr<Shader> shader(std::make_shared<Shader>(type));
   if (!shader->compile(getDefaultShaderSource(type))) {
      LOG_ERROR("Error compiling default " << getShaderTypeName(type) << " shader. Error message: \""
                  << getShaderCompileError(shader) << "\"");
   }

   return shader;
}

SPtr<ShaderProgram> createDefaultShaderProgram(const SPtr<Shader> &defaultVertexShader,
                                               const SPtr<Shader> &defaultFragmentShader) {
   SPtr<ShaderProgram>defaultShaderProgram(std::make_shared<ShaderProgram>());

   int shaderCount = 0;
   if (defaultVertexShader) {
      defaultShaderProgram->attach(defaultVertexShader);
      ++shaderCount;
   }
   if (defaultFragmentShader) {
      defaultShaderProgram->attach(defaultFragmentShader);
      ++shaderCount;
   }

   if (shaderCount >= 2) {
      if (!defaultShaderProgram->link()) {
         LOG_ERROR("Error linking default shader program. Error message: \""
                   << getShaderLinkError(defaultShaderProgram) << "\"");
      }
   } else {
      LOG_ERROR("Not enough shaders to link default shader program");
   }

   return defaultShaderProgram;
}

} // namespace

SPtr<Shader> ShaderLoader::getDefaultShader(const GLenum type) {
   switch (type) {
      case GL_VERTEX_SHADER:
         if (!defaultVertexShader) {
            defaultVertexShader = createDefaultShader(type);
         }
         return defaultVertexShader;
      case GL_GEOMETRY_SHADER:
         if (!defaultGeometryShader) {
            defaultGeometryShader = createDefaultShader(type);
         }
         return defaultGeometryShader;
      case GL_FRAGMENT_SHADER:
         if (!defaultFragmentShader) {
            defaultFragmentShader = createDefaultShader(type);
         }
         return defaultFragmentShader;
         break;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
         return nullptr;
   }
}

SPtr<ShaderProgram> ShaderLoader::getDefaultShaderProgram() {
   if (!defaultShaderProgram) {
      defaultShaderProgram = createDefaultShaderProgram(getDefaultShader(GL_VERTEX_SHADER),
                                                        getDefaultShader(GL_FRAGMENT_SHADER));
   }

   return defaultShaderProgram;
}

SPtr<Shader> ShaderLoader::loadShader(const std::string &fileName, const GLenum type) {
   ASSERT(type == GL_VERTEX_SHADER || type == GL_GEOMETRY_SHADER || type == GL_FRAGMENT_SHADER,
          "Invalid shader type: %i", type);

   ShaderMap::iterator location(shaderMap.find(fileName));
   if (location != shaderMap.end()) {
      return location->second;
   }

   std::string source;
   if (!IOUtils::readTextFile(fileName, source)) {
      LOG_WARNING("Unable to load shader from file \"" << fileName << "\", reverting to default");
      return getDefaultShader(type);
   }

   SPtr<Shader> shader(std::make_shared<Shader>(type));
   if (!shader->compile(source.c_str())) {
      LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \""
                  << fileName << "\", reverting to default. Error message: \""
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
   std::string vertexFileName = fileName + kVertexExtension;
   std::string geometryFileName = fileName + kGeometryExtension;
   std::string fragmentFileName = fileName + kFragmentExtension;

   int shaderCount = 0;
   if (IOUtils::canRead(vertexFileName)) {
      shaderProgram->attach(loadShader(vertexFileName, GL_VERTEX_SHADER));
      ++shaderCount;
   }

   if (IOUtils::canRead(geometryFileName)) {
      shaderProgram->attach(loadShader(geometryFileName, GL_GEOMETRY_SHADER));
      ++shaderCount;
   }

   if (IOUtils::canRead(fragmentFileName)) {
      shaderProgram->attach(loadShader(fragmentFileName, GL_FRAGMENT_SHADER));
      ++shaderCount;
   }

   if (shaderCount >= 2) {
      if (!shaderProgram->link()) {
         LOG_WARNING("Unable to link '" << fileName
                     << "' shader program, reverting to default. Error message: \""
                     << getShaderLinkError(shaderProgram) << "\"");
         shaderProgram = getDefaultShaderProgram();
      }
   } else {
      LOG_WARNING("Not enough shaders to link program, reverting to default");
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
      if (!IOUtils::readTextFile(fileName, source)) {
         LOG_WARNING("Unable to load shader from file \"" << fileName << "\", not reloading");
         continue;
      }

      if (!shader->compile(source.c_str())) {
         LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \""
                     << fileName << "\", reverting to default. Error message: \""
                     << getShaderCompileError(shader) << "\"");

         const char *defaultSource = getDefaultShaderSource(shader->getType());
         if (!shader->compile(defaultSource)) {
            LOG_ERROR("Error compiling default " << getShaderTypeName(shader->getType())
                        << " shader. Error message: \"" << getShaderCompileError(shader) << "\"");
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
