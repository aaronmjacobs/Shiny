#ifndef SHINY_CONTROLLER_H
#define SHINY_CONTROLLER_H

#include "Shiny/Defines.h"
#include "Shiny/GLIncludes.h"

#include "Shiny/Input/InputDevice.h"

namespace Shiny {

namespace {

SHINYAPI const int kMaxControllers = GLFW_JOYSTICK_LAST + 1;

} // namespace

struct ControllerMap;

struct SHINYAPI ControllerValues {
   // Axes
   float leftThumbHorizontal;
   float leftThumbVertical;

   float rightThumbHorizontal;
   float rightThumbVertical;

   float leftTriggerAxis;
   float rightTriggerAxis;

   // Buttons
   bool dPadLeft;
   bool dPadRight;
   bool dPadUp;
   bool dPadDown;

   bool faceLeft;
   bool faceRight;
   bool faceUp;
   bool faceDown;

   bool leftTrigger;
   bool rightTrigger;

   bool leftBumper;
   bool rightBumper;

   bool leftThumb;
   bool rightThumb;

   bool leftMenu;
   bool rightMenu;
};

class SHINYAPI Controller : public InputDevice {
protected:
   GLFWwindow* const window;
   const int controllerNum;
   const ControllerMap *map;
   ControllerValues inputValues;

   void updateMap();

   void handleSpecialCases(const unsigned char *buttons, const float *axes, int buttonCount, int axisCount);

public:
   Controller(GLFWwindow* const window, const int controllerNum);

   virtual ~Controller();

   virtual void poll() override;

   bool connected() const;

   const ControllerValues& values() const;
};

} // namespace Shiny

#endif
