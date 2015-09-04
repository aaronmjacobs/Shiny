#ifndef SHINY_CONTROLLER_MAP_H
#define SHINY_CONTROLLER_MAP_H

#include <string>

namespace Shiny {

struct ControllerAxis {
   int index;
   bool centerDeadzone;
   bool invert;
};

struct ControllerMap {
   // Name
   const char *name;

   // Settings
   float lookSensitivity;
   float deadzone;

   // Axes
   ControllerAxis leftThumbHorizontal;
   ControllerAxis leftThumbVertical;

   ControllerAxis rightThumbHorizontal;
   ControllerAxis rightThumbVertical;

   ControllerAxis leftTriggerAxis;
   ControllerAxis rightTriggerAxis;

   ControllerAxis dPadHorizontal;
   ControllerAxis dPadVertical;

   // Buttons
   int dPadLeft;
   int dPadRight;
   int dPadUp;
   int dPadDown;

   int faceLeft;
   int faceRight;
   int faceUp;
   int faceDown;

   int leftTrigger;
   int rightTrigger;

   int leftBumper;
   int rightBumper;

   int leftThumb;
   int rightThumb;

   int leftMenu;
   int rightMenu;
};

namespace ControllerType {

const ControllerMap* none();

const ControllerMap* guess(const std::string &name, int numButtons, int numAxes);

} // namespace ControllerType

} // namespace Shiny

#endif
