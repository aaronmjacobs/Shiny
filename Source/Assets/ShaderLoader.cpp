#include "Shiny/Log.h"
#include "Shiny/Shiny.h"
#include "Shiny/ShinyAssert.h"
#include "Shiny/Assets/ShaderLoader.h"
#include "Shiny/Graphics/Shader.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Platform/IOUtils.h"
#include "Shiny/Platform/OSUtils.h"

#include <set>
#include <sstream>
#include <unordered_map>

#if defined(GLSL)
#  undef GLSL
#endif
#define GLSL(source) "#version 330 core\n" #source

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
   SPtr<Shader> shader = std::make_shared<Shader>(type);
   if (!shader->compile(getDefaultShaderSource(type))) {
      LOG_ERROR("Error compiling default " << getShaderTypeName(type) << " shader. Error message: \""
                  << getShaderCompileError(shader) << "\"");
   }

   return shader;
}

SPtr<ShaderProgram> createDefaultShaderProgram(const SPtr<Shader>& defaultVertexShader,
                                               const SPtr<Shader>& defaultFragmentShader) {
   SPtr<ShaderProgram> defaultShaderProgram = std::make_shared<ShaderProgram>();

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

void replaceAll(std::string& string, const std::string& search, const std::string& replace) {
   for (std::size_t pos = 0; ; pos += replace.length()) {
      pos = string.find(search, pos);
      if (pos == std::string::npos) {
         break;
      }

      string.erase(pos, search.length());
      string.insert(pos, replace);
   }
}

bool findInclude(const std::string& source, std::size_t& includeStartPos, std::size_t& includeEndPos, std::size_t& pathStartPos, std::size_t& pathEndPos) {
   includeStartPos = includeEndPos = pathStartPos = pathEndPos = std::string::npos;

   includeStartPos = source.find("#include");
   if (includeStartPos == std::string::npos) {
      return false;
   }

   std::size_t openingQuotePos = source.find("\"", includeStartPos + 1);
   if (openingQuotePos == std::string::npos) {
      return false;
   }

   std::size_t closingQuotePos = source.find("\"", openingQuotePos + 1);
   if (closingQuotePos == std::string::npos) {
      return false;
   }

   includeEndPos = closingQuotePos + 1;
   pathStartPos = openingQuotePos + 1;
   pathEndPos = closingQuotePos - 1;

   return true;
}

std::string loadShaderSource(const Path& path, const std::unordered_map<std::string, std::string>& definitions, std::set<Path>& loadedFiles) {
   // Prevent files from being included multiple times
   if (loadedFiles.count(path)) {
      return "";
   }
   loadedFiles.insert(path);

   std::string source;
   if (!IOUtils::readTextFile(path, source)) {
      LOG_WARNING("Unable to load shader from file \"" << path << "\"");
      return "";
   }

   Path directory = path.getDirectory();

   // Recursively handle all #include directives
   std::size_t includeStartPos, includeEndPos, pathStartPos, pathEndPos;
   while (findInclude(source, includeStartPos, includeEndPos, pathStartPos, pathEndPos)) {
      Path includePath(directory, source.substr(pathStartPos, (pathEndPos - pathStartPos) + 1));

      source.erase(includeStartPos, (includeEndPos - includeStartPos) + 1);
      source.insert(includeStartPos, loadShaderSource(includePath, definitions, loadedFiles));
   }

   // Clean up extra #version directives
   std::size_t versionPos = source.find("#version");
   while ((versionPos = source.find("#version", versionPos + 1)) != std::string::npos) {
      std::size_t endOfLinePos = source.find("\n", versionPos);
      if (endOfLinePos == std::string::npos) {
         LOG_WARNING("Unable to remove #version directive (no newline)");
         break;
      }
      source.erase(versionPos, (endOfLinePos - versionPos) + 1);
   }

   // Replace all defined values
   for (const auto& pair : definitions) {
      replaceAll(source, pair.first, pair.second);
   }

   return source;
}

std::string loadShaderSource(const Path& path, const std::unordered_map<std::string, std::string>& definitions) {
   std::set<Path> loadedFiles;
   return loadShaderSource(path, definitions, loadedFiles);
}

} // namespace

SPtr<Shader> ShaderLoader::loadShader(const Path& path, const GLenum type, const std::unordered_map<std::string, std::string>& definitions) {
   ASSERT(type == GL_VERTEX_SHADER || type == GL_GEOMETRY_SHADER || type == GL_FRAGMENT_SHADER,
          "Invalid shader type: %i", type);

   ShaderPermutation shaderPermutation;
   shaderPermutation.path = path;
   shaderPermutation.definitions = definitions;

   auto location = shaderMap.find(shaderPermutation);
   if (location != shaderMap.end()) {
      return location->second;
   }

   std::string source = loadShaderSource(path, definitions);
   if (source.empty()) {
      LOG_WARNING("Reverting to default shader (instead of " << path << ")");
      return getDefaultShader(type);
   }

   SPtr<Shader> shader = std::make_shared<Shader>(type);
   if (!shader->compile(source.c_str())) {
      LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \""
                  << path << "\", reverting to default. Error message: \""
                  << getShaderCompileError(shader) << "\"");
      shader = getDefaultShader(type);
   }

   shaderMap.insert({ shaderPermutation, shader });
   return shader;
}

SPtr<ShaderProgram> ShaderLoader::loadShaderProgram(const Path& path, const std::unordered_map<std::string, std::string>& definitions) {
   ShaderPermutation shaderPermutation;
   shaderPermutation.path = path;
   shaderPermutation.definitions = definitions;

   auto location = shaderProgramMap.find(shaderPermutation);
   if (location != shaderProgramMap.end()) {
      return location->second;
   }

   SPtr<ShaderProgram> shaderProgram = std::make_shared<ShaderProgram>();
   Path vertexPath = path + kVertexExtension;
   Path geometryPath = path + kGeometryExtension;
   Path fragmentPath = path + kFragmentExtension;

   int shaderCount = 0;
   if (IOUtils::canRead(vertexPath)) {
      shaderProgram->attach(loadShader(vertexPath, GL_VERTEX_SHADER, definitions));
      ++shaderCount;
   }

   if (IOUtils::canRead(geometryPath)) {
      shaderProgram->attach(loadShader(geometryPath, GL_GEOMETRY_SHADER, definitions));
      ++shaderCount;
   }

   if (IOUtils::canRead(fragmentPath)) {
      shaderProgram->attach(loadShader(fragmentPath, GL_FRAGMENT_SHADER, definitions));
      ++shaderCount;
   }

   if (shaderCount >= 2) {
      if (!shaderProgram->link()) {
         LOG_WARNING("Unable to link '" << path
                     << "' shader program, reverting to default. Error message: \""
                     << getShaderLinkError(shaderProgram) << "\"");
         shaderProgram = getDefaultShaderProgram();
      }
   } else {
      LOG_WARNING("Not enough shaders to link program, reverting to default");
      shaderProgram = getDefaultShaderProgram();
   }

   shaderProgramMap.insert({ shaderPermutation, shaderProgram });
   return shaderProgram;
}

void ShaderLoader::reloadShaders() {
   // TODO Only reload if files have been updated (check file modification time)

   for (const auto& pair : shaderMap) {
      const ShaderPermutation& shaderPermutation = pair.first;
      const SPtr<Shader>& shader = pair.second;

      std::string source;
      if (!IOUtils::readTextFile(shaderPermutation.path, source)) {
         LOG_WARNING("Unable to load shader from file \"" << shaderPermutation.path << "\", not reloading");
         continue;
      }

      if (!shader->compile(source.c_str())) {
         LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \""
                     << shaderPermutation.path << "\", reverting to default. Error message: \""
                     << getShaderCompileError(shader) << "\"");

         const char* defaultSource = getDefaultShaderSource(shader->getType());
         if (!shader->compile(defaultSource)) {
            LOG_ERROR("Error compiling default " << getShaderTypeName(shader->getType())
                        << " shader. Error message: \"" << getShaderCompileError(shader) << "\"");
         }
      }
   }

   for (const auto& pair : shaderProgramMap) {
      const ShaderPermutation& shaderPermutation = pair.first;
      const SPtr<ShaderProgram>& shaderProgram = pair.second;

      if (!shaderProgram->link()) {
         LOG_WARNING("Unable to link '" << shaderPermutation.path << "' shader program. Error message: \""
                     << getShaderLinkError(shaderProgram) << "\"");
      }
   }
}

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

} // namespace Shiny
