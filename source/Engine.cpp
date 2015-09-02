#include "Shiny/Engine.h"
#include "Shiny/GLIncludes.h"
#include "Shiny/Shiny.h"
#include "Shiny/ShinyAssert.h"

#include <algorithm>

namespace Shiny {

namespace {

const double kPhysicsFrameRate = 60.0;
const double kMaxFrameTime = 0.25;

} // namespace

// static
void Engine::windowCloseCallback(GLFWwindow *window) {
   Engine *engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
   engine->onWindowClose();
}

// static
void Engine::windowSizeCallback(GLFWwindow *window, int width, int height) {
   Engine *engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
   engine->onWindowSizeChange(width, height);
}

// static
void Engine::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
   Engine *engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
   engine->onFramebufferSizeChange(width, height);
}

// static
void Engine::windowPosCallback(GLFWwindow *window, int xPos, int yPos) {
   Engine *engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
   engine->onWindowMove(xPos, yPos);
}

// static
void Engine::windowIconifyCallback(GLFWwindow *window, int iconified) {
   Engine *engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
   engine->onWindowIconifyChange(!!iconified);
}

// static
void Engine::windowFocusCallback(GLFWwindow *window, int focused) {
   Engine *engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
   engine->onWindowFocusChange(!!focused);
}

Engine::Engine()
   : window(nullptr) {
}

Engine::~Engine() {
}

void Engine::onWindowClose() {
}

void Engine::onWindowSizeChange(int width, int height) {
}

void Engine::onFramebufferSizeChange(int width, int height) {
}

void Engine::onWindowMove(int xPos, int yPos) {
}

void Engine::onWindowIconifyChange(bool iconified) {
}

void Engine::onWindowFocusChange(bool focused) {
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

   glfwSetWindowUserPointer(window.get(), this);

   glfwSetWindowCloseCallback(window.get(), Engine::windowCloseCallback);
   glfwSetWindowSizeCallback(window.get(), Engine::windowSizeCallback);
   glfwSetFramebufferSizeCallback(window.get(), Engine::framebufferSizeCallback);
   glfwSetWindowPosCallback(window.get(), Engine::windowPosCallback);
   glfwSetWindowIconifyCallback(window.get(), Engine::windowIconifyCallback);
   glfwPollEvents(); // Ignore the first window focus callback
   glfwSetWindowFocusCallback(window.get(), Engine::windowFocusCallback);

   return Result::kOK;
}

void Engine::shutDown() {
   glfwSetWindowShouldClose(window.get(), true);
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
