#include "Engine.h"

#include "GLIncludes.h"
#include "Shiny.h"
#include "ShinyAssert.h"

#include <algorithm>

namespace Shiny {

namespace {

const double kPhysicsFrameRate = 60.0;
const double kMaxFrameTime = 0.25;

} // namespace

Engine::Engine()
   : window(nullptr) {
}

Engine::~Engine() {
}

Result Engine::startUp(int windowWidth, int windowHeight, const char *windowName) {
   if (windowWidth <= 0 || windowHeight <= 0 || !windowName) {
      return Result::kWindowParams;
   }

   WindowPtr newWindow(WindowPtr(glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr),
                                 glfwDestroyWindow));
   if (!newWindow) {
      return Result::kCreateWindow;
   }

   glfwMakeContextCurrent(newWindow.get());
   context.makeCurrent();
   glfwSwapInterval(1); // VSYNC

   Result glResult = loadGL();
   if (glResult != Result::kOK) {
      return glResult;
   }

   window = std::move(newWindow);

   // TODO Callbacks and such

   return Result::kOK;
}

void Engine::run() {
   ASSERT(window, "Trying to run engine before starting up");

   // Timing values
   const double dt = 1.0 / kPhysicsFrameRate;
   double lastTime = glfwGetTime();

   // Make sure we tick at least once before rendering, to allow things to be set up
   double accumulator = dt;

   while (!glfwWindowShouldClose(window.get())) {
      double now = glfwGetTime();
      // Cap the frame time to prevent spiraling
      double frameTime = std::min(now - lastTime, kMaxFrameTime);
      lastTime = now;

      accumulator += frameTime;
      while (accumulator >= dt) {
         tick(dt);
         accumulator -= dt;
      }

      render();

      glfwSwapBuffers(window.get());
      glfwPollEvents();
   }
}

} // namespace Shiny
