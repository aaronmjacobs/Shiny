#include "Shiny/Engine.h"
#include "Shiny/Shiny.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Input/Keyboard.h"
#include "Shiny/Input/Mouse.h"

#include <GLFW/glfw3.h>

#include <algorithm>

namespace Shiny {

namespace {

const double kPhysicsFrameRate = 60.0;
const double kMaxFrameTime = 0.25;

// Loads OpenGL - must be called after an OpenGL context is created
Engine::Result loadGL() {
   static bool loaded = false;

   if (loaded) {
      return Engine::Result::kOK;
   }

   if (!gladLoadGL()) {
      return Engine::Result::kGladLoad;
   }

   loaded = true;
   return Engine::Result::kOK;
}

} // namespace

// static
const char* Engine::errorString(Result result) {
   switch (result) {
      case Result::kOK:
         return "No error";
      case Result::kGladLoad:
         return "Unable to load OpenGL via glad";
      case Result::kWindowParams:
         return "Invalid window parameters";
      case Result::kCreateWindow:
         return "Unable to create window";
      case Result::kAudioInit:
         return "Unable to initialize audio system";
      default:
         return "Unknown error";
   }
}

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
   : window(nullptr), running(false), runningTime(0.0f) {
   controllers.fill(nullptr);
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

void Engine::pollInput() {
   keyboard->poll();
   mouse->poll();

   for (const SPtr<Controller> &controller : controllers) {
      controller->poll();
   }
}

Engine::Result Engine::startUp(int windowWidth, int windowHeight, const char *windowName) {
   ASSERT(!running, "Trying to start up engine that is already running");

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
   context.poll();

   window = std::move(newWindow);

   glfwSetWindowUserPointer(window.get(), this);

   glfwSetWindowCloseCallback(window.get(), Engine::windowCloseCallback);
   glfwSetWindowSizeCallback(window.get(), Engine::windowSizeCallback);
   glfwSetFramebufferSizeCallback(window.get(), Engine::framebufferSizeCallback);
   glfwSetWindowPosCallback(window.get(), Engine::windowPosCallback);
   glfwSetWindowIconifyCallback(window.get(), Engine::windowIconifyCallback);
   glfwPollEvents(); // Ignore the first window focus callback
   glfwSetWindowFocusCallback(window.get(), Engine::windowFocusCallback);

   keyboard = std::make_shared<Keyboard>(window.get());
   mouse = std::make_shared<Mouse>(window.get());
   for (size_t i = 0; i < controllers.size(); ++i) {
      controllers[i] = std::make_shared<Controller>(window.get(), i);
   }
   pollInput();

   runningTime = 0.0f;

   if (audioSystem.startUp() != AudioSystem::Result::kOK) {
      return Result::kAudioInit;
   }

   return Result::kOK;
}

void Engine::shutDown() {
   audioSystem.shutDown();

   glfwSetWindowShouldClose(window.get(), true);
}

void Engine::run() {
   ASSERT(window, "Trying to run engine before starting up");
   ASSERT(!running, "Trying to run engine that is already running");

   running = true;

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
         tick(static_cast<float>(dt));
         runningTime += static_cast<float>(dt);
         accumulator -= dt;
      }

      render();

      glfwSwapBuffers(window.get());
      glfwPollEvents();

      pollInput();
   }

   running = false;
}

SPtr<const Keyboard> Engine::getKeyboard() const {
   return keyboard;
}

SPtr<const Mouse> Engine::getMouse() const {
   return mouse;
}

SPtr<const Controller> Engine::getController(int which) const {
   ASSERT(which >= 0 && static_cast<size_t>(which) < controllers.size(), "Invalid controller number: %d", which);
   return controllers[which];
}

bool Engine::isRunning() const {
   return running;
}

float Engine::getRunningTime() const {
   return runningTime;
}

} // namespace Shiny
