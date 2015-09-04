#ifndef SHINY_MOUSE_H
#define SHINY_MOUSE_H

#include "Shiny/Defines.h"

#include "Shiny/Input/InputDevice.h"

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

class SHINYAPI Mouse : public InputDevice {
protected:
   GLFWwindow* const window;
   double x, y;
   bool left, middle, right;

public:
   Mouse(GLFWwindow* const window);

   virtual ~Mouse();

   virtual void poll() override;

   double xPos() const {
      return x;
   }

   double yPos() const {
      return y;
   }

   bool leftButton() const {
      return left;
   }

   bool middleButton() const {
      return middle;
   }

   bool rightButton() const {
      return right;
   }
};

} // namespace Shiny

#endif
