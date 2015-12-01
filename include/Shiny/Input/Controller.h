#ifndef SHINY_CONTROLLER_H
#define SHINY_CONTROLLER_H

#include "Shiny/Input/InputDevice.h"

#include <GLFW/glfw3.h>

namespace Shiny {

namespace {

const int kMaxControllers = GLFW_JOYSTICK_LAST + 1;

} // namespace

struct ControllerMap;

struct ControllerValues {
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

class Controller : public InputDevice {
protected:
   GLFWwindow* const window;
   const int controllerNum;
   const ControllerMap *map;
   ControllerValues inputValues, lastInputValues, newButtonInputValues;

   void updateMap();

   void handleSpecialCases(const unsigned char *buttons, const float *axes, int buttonCount, int axisCount);

public:
   Controller(GLFWwindow* const window, const int controllerNum);

   virtual ~Controller();

   virtual void poll() override;

   bool connected() const;

   const ControllerValues& values(bool onlyNewButtons = false) const;
};

} // namespace Shiny

#endif
