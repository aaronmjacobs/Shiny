#include "Shiny/GLIncludes.h"
#include "Shiny/Log.h"
#include "Shiny/Shiny.h"

#include "Shiny/Platform/OSUtils.h"

namespace Shiny {

namespace {

void errorCallback(int error, const char* description) {
   LOG_FATAL("GLFW error " << error << ": " << description);
}

} // namespace

const char* errorString(Result result) {
   switch (result) {
      case Result::kOK:
         return "No error";
      case Result::kWorkingDirectory:
         return "Unable to set program working directory";
      case Result::kGlfwInit:
         return "Unable to initialize GLFW";
      case Result::kGladLoad:
         return "Unable to load OpenGL via glad";
      case Result::kWindowParams:
         return "Invalid window parameters";
      case Result::kCreateWindow:
         return "Unable to create window";
      default:
         return "Unknown error";
   }
}

Result init() {
   if (!OSUtils::fixWorkingDirectory()) {
      return Result::kWorkingDirectory;
   }

   glfwSetErrorCallback(errorCallback);

   if (!glfwInit()) {
      return Result::kGlfwInit;
   }

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   return Result::kOK;
}

void terminate() {
   glfwTerminate();
}

Result loadGL() {
   static bool loaded = false;

   if (loaded) {
      return Result::kOK;
   }

   if (!gladLoadGL()) {
      return Result::kGladLoad;
   }

   loaded = true;
   return Result::kOK;
}

} // namespace Shiny
