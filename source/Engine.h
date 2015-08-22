#ifndef SHINY_ENGINE_H
#define SHINY_ENGINE_H

#include "Pointers.h"
#include "Shiny.h"

#include <functional>

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

typedef UPtr<GLFWwindow, std::function<void(GLFWwindow*)>> WindowPtr;

class Engine {
protected:
   WindowPtr window;

public:
   Engine();

   virtual ~Engine();

   Result startUp(int windowWidth, int windowHeight, const char *windowName);

   void run();

   virtual void tick(const float dt) = 0;

   virtual void render() = 0;
};

} // namespace Shiny

#endif
