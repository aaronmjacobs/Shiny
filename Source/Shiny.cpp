#include "Shiny/Shiny.h"
#include "Shiny/ShinyAssert.h"
#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Platform/OSUtils.h"

#include <cxxopts.hpp>
#include <GLFW/glfw3.h>

namespace Shiny {

namespace {

const char* kDefaultDataFolderPath = "Data";
std::string dataFolderPath = kDefaultDataFolderPath;

void glfwErrorCallback(int error, const char* description) {
   ASSERT(false, "GLFW error %d: %s", error, description);
}

#if SHINY_DEBUG
void gladPostCallback(const char* name, void* funcptr, int numArgs, ...) {
   (void)funcptr;
   (void)numArgs;

   GLenum errorCode = glad_glGetError();
   ASSERT(errorCode == GL_NO_ERROR, "OpenGL error %d in %s", errorCode, name);
}
#endif // SHINY_DEBUG

} // namespace

const char* errorString(Result result) {
   switch (result) {
      case Result::kOK:
         return "No error";
      case Result::kWorkingDirectory:
         return "Unable to set program working directory";
      case Result::kGlfwInit:
         return "Unable to initialize GLFW";
      default:
         return "Unknown error";
   }
}

Result init(int argc, char* argv[]) {
   static const char* kDataPathOption = "data_path";

   cxxopts::Options options("Shiny", "Simple game engine");
   options.add_options()(kDataPathOption, "Path to the data folder", cxxopts::value<std::string>()->default_value(kDefaultDataFolderPath));
   options.parse(argc, argv);
   dataFolderPath = options[kDataPathOption].as<std::string>();

   if (dataFolderPath.length() > 1) {
      char lastChar = dataFolderPath[dataFolderPath.length() - 1];
      if (lastChar == '/' || lastChar == '\\') {
         dataFolderPath.pop_back();
      }
   }

   if (!OSUtils::fixWorkingDirectory()) {
      return Result::kWorkingDirectory;
   }

   glfwSetErrorCallback(glfwErrorCallback);
   if (!glfwInit()) {
      return Result::kGlfwInit;
   }

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#if SHINY_DEBUG
   glad_set_post_callback(gladPostCallback);
#endif // SHINY_DEBUG

   return Result::kOK;
}

void terminate() {
   glfwTerminate();
}

const std::string& dataPath() {
   return dataFolderPath;
}

} // namespace Shiny
