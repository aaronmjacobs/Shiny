#ifndef SHINY_MOUSE_H
#define SHINY_MOUSE_H

#include "Shiny/Input/InputDevice.h"

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

class Mouse : public InputDevice {
public:
   enum class Mode {
      kNormal,
      kHidden,
      kDisabled
   };

protected:
   GLFWwindow* const window;
   double x, y, lastX, lastY;
   bool left, middle, right, lastLeft, lastMiddle, lastRight;

public:
   Mouse(GLFWwindow* const window);

   virtual ~Mouse();

   virtual void poll() override;

   Mode getMode() const;

   void setMode(Mode mode) const;

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
