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
   double x, y, lastX, lastY;
   bool left, middle, right, lastLeft, lastMiddle, lastRight;

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

   double xDiff() const {
      return x - lastX;
   }

   double yDiff() const {
      return y - lastY;
   }

   bool leftButton(bool onlyNew = false) const {
      return onlyNew ? left && !lastLeft : left;
   }

   bool middleButton(bool onlyNew = false) const {
      return onlyNew ? middle && !lastMiddle : middle;
   }

   bool rightButton(bool onlyNew = false) const {
      return onlyNew ? right && !lastRight : right;
   }
};

} // namespace Shiny

#endif
