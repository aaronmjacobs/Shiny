#ifndef SHINY_ENGINE_H
#define SHINY_ENGINE_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"
#include "Shiny/Shiny.h"

#include "Shiny/Graphics/Context.h"

#include <functional>

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

typedef UPtr<GLFWwindow, std::function<void(GLFWwindow*)>> WindowPtr;

class SHINYAPI Engine {
private:
   static void windowCloseCallback(GLFWwindow *window);

   static void windowSizeCallback(GLFWwindow *window, int width, int height);

   static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

   static void windowPosCallback(GLFWwindow *window, int xPos, int yPos);

   static void windowIconifyCallback(GLFWwindow *window, int iconified);

   static void windowFocusCallback(GLFWwindow *window, int focused);

protected:
   WindowPtr window;
   Context context;
   bool running;
   float runningTime;

   virtual void onWindowClose();

   virtual void onWindowSizeChange(int width, int height);

   virtual void onFramebufferSizeChange(int width, int height);

   virtual void onWindowMove(int xPos, int yPos);

   virtual void onWindowIconifyChange(bool iconified);

   virtual void onWindowFocusChange(bool focused);

public:
   Engine();

   virtual ~Engine();

   Result startUp(int windowWidth, int windowHeight, const char *windowName);

   void shutDown();

   void run();

   virtual void tick(const float dt) = 0;

   virtual void render() = 0;

   bool isRunning() const;

   float getRunningTime() const;
};

} // namespace Shiny

#endif
