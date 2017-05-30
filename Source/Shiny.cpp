#include "Shiny/Shiny.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/OpenGL.h"

#include "Shiny/Platform/OSUtils.h"

#include <GLFW/glfw3.h>

namespace Shiny {

namespace {

void errorCallback(int error, const char* description) {
   ASSERT(false, "GLFW error %d: %s", error, description);
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
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   return Result::kOK;
}

void terminate() {
   glfwTerminate();
}

} // namespace Shiny
