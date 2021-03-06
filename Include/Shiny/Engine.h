#ifndef SHINY_ENGINE_H
#define SHINY_ENGINE_H

#include "Shiny/Pointers.h"
#include "Shiny/Shiny.h"

#include "Shiny/Audio/AudioSystem.h"

#include "Shiny/Graphics/Context.h"

#include "Shiny/Input/Controller.h"

#include <array>
#include <functional>

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

class Keyboard;
class Mouse;

typedef UPtr<GLFWwindow, std::function<void(GLFWwindow*)>> WindowPtr;

class Engine {
public:
   enum class Result {
      kOK,
      kGladLoad,
      kWindowParams,
      kCreateWindow,
      kAudioInit
   };

   static const char* errorString(Result result);

private:
   static void windowCloseCallback(GLFWwindow *window);

   static void windowSizeCallback(GLFWwindow *window, int width, int height);

   static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

   static void windowPosCallback(GLFWwindow *window, int xPos, int yPos);

   static void windowIconifyCallback(GLFWwindow *window, int iconified);

   static void windowFocusCallback(GLFWwindow *window, int focused);

protected:
   WindowPtr window;
   AudioSystem audioSystem;
   Context context;
   SPtr<Keyboard> keyboard;
   SPtr<Mouse> mouse;
   std::array<SPtr<Controller>, kMaxControllers> controllers;
   bool running;
   float runningTime;

   virtual void onWindowClose();

   virtual void onWindowSizeChange(int width, int height);

   virtual void onFramebufferSizeChange(int width, int height);

   virtual void onWindowMove(int xPos, int yPos);

   virtual void onWindowIconifyChange(bool iconified);

   virtual void onWindowFocusChange(bool focused);

   void pollInput();

public:
   Engine();

   virtual ~Engine() = default;

   Result startUp(int windowWidth, int windowHeight, const char *windowName);

   void shutDown();

   void run();

   virtual void tick(const float dt) = 0;

   virtual void render() = 0;

   SPtr<const Keyboard> getKeyboard() const;

   SPtr<const Mouse> getMouse() const;

   SPtr<const Controller> getController(int which) const;

   bool isRunning() const;

   float getRunningTime() const;
};

} // namespace Shiny

#endif
