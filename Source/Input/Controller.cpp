#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/OpenGL.h"

#include "Shiny/Input/Controller.h"
#include "Shiny/Input/ControllerMap.h"

#include <glm/glm.hpp>

namespace Shiny {

namespace {

const float kAxisMin = -1.0f;
const float kAxisMax = 1.0f;
const float kAxisCenter = 0.0f;

float applyEdgeDeadzone(float f, float minVal, float maxVal, float deadzone) {
   float center = (minVal + maxVal) / 2.0f;
   float fullScale = maxVal - minVal;
   float deadzonedScale = (maxVal - deadzone) - (minVal + deadzone);
   if (deadzonedScale == 0.0f) {
      return f;
   }

   float scale = fullScale / deadzonedScale;
   return glm::clamp(((f - center) * scale) + center, minVal, maxVal);
}

float applyDeadzone(float f, float deadzone, bool center) {
   float minVal = kAxisMin;
   float maxVal = kAxisMax;

   if (center) {
      if (f > kAxisCenter) {
         minVal = kAxisCenter;
      } else {
         maxVal = kAxisCenter;
      }
   }

   return applyEdgeDeadzone(f, minVal, maxVal, deadzone);
}

float axisValue(const float *axes, const int axisCount, const ControllerAxis &axis, const float deadzone) {
   // Make sure the mapped index is valid
   if (axis.index < 0 || axis.index >= axisCount) {
      return kAxisCenter;
   }

   return applyDeadzone(axes[axis.index], deadzone, axis.centerDeadzone) * (axis.invert ? -1.0f : 1.0f);
}

bool buttonValue(const unsigned char *buttons, const int buttonCount, const int button) {
   // Make sure the mapped index is valid
   if (button < 0 || button >= buttonCount) {
      return false;
   }

   return buttons[button] == GLFW_PRESS;
}

} // namespace

Controller::Controller(GLFWwindow* const window, const int controllerNum)
   : window(window), controllerNum(controllerNum), map(ControllerType::none()), inputValues({ 0 }),
     lastInputValues({ 0 }), newButtonInputValues({ 0 }) {
   ASSERT(controllerNum >= GLFW_JOYSTICK_1 && controllerNum <= GLFW_JOYSTICK_LAST, "Invalid controller number");

   // Set initial state
   poll();
   lastInputValues = inputValues;
   newButtonInputValues = { 0 };
}

void Controller::updateMap() {
   if (!connected()) {
      if (map->name) {
         // Controller used to be connected, but is not any more
         map = ControllerType::none();
      }
      return;
   }

   int buttonCount = 0, axisCount = 0;
   glfwGetJoystickButtons(controllerNum, &buttonCount);
   glfwGetJoystickAxes(controllerNum, &axisCount);
   const char *name = glfwGetJoystickName(controllerNum);

   const ControllerMap *newMap(ControllerType::guess(name, buttonCount, axisCount));
   if (newMap->name != map->name) {
      // New controller type was connected
      map = newMap;
   }
}

void Controller::handleSpecialCases(const unsigned char *buttons, const float *axes, int buttonCount, int axisCount) {
   // Triggers only treated as buttons, and not as axes
   if (map->leftTriggerAxis.index < 0) {
      inputValues.leftTriggerAxis = inputValues.leftTrigger ? kAxisMax : kAxisMin;
   }
   if (map->rightTriggerAxis.index < 0) {
      inputValues.rightTriggerAxis = inputValues.rightTrigger ? kAxisMax : kAxisMin;
   }

   // Triggers share one axis
   if (map->leftTriggerAxis.index == map->rightTriggerAxis.index && map->leftTriggerAxis.index >= 0) {
      inputValues.leftTriggerAxis = glm::max(inputValues.leftTriggerAxis, kAxisCenter) * 2.0f - kAxisMax;
      inputValues.rightTriggerAxis = glm::max(inputValues.rightTriggerAxis, kAxisCenter) * 2.0f - kAxisMax;
   }

   // Triggers not treated as buttons
   if (map->leftTrigger < 0) {
      inputValues.leftTrigger = inputValues.leftTriggerAxis > kAxisCenter;
   }
   if (map->rightTrigger < 0) {
      inputValues.rightTrigger = inputValues.rightTriggerAxis > kAxisCenter;
   }

   // dPad treated as two axes
   float dPadHorizontal = axisValue(axes, axisCount, map->dPadHorizontal, map->deadzone);
   float dPadVertical = axisValue(axes, axisCount, map->dPadVertical, map->deadzone);
   if (map->dPadLeft < 0) {
      inputValues.dPadLeft = dPadHorizontal < kAxisCenter;
   }
   if (map->dPadRight < 0) {
      inputValues.dPadRight = dPadHorizontal > kAxisCenter;
   }
   if (map->dPadUp < 0) {
      inputValues.dPadUp = dPadVertical < kAxisCenter;
   }
   if (map->dPadDown < 0) {
      inputValues.dPadDown = dPadVertical > kAxisCenter;
   }
}

#define SHINY_ASSIGN_AXIS(name) do {\
inputValues.name = axisValue(axes, axisCount, map->name, map->deadzone);\
newButtonInputValues.name = inputValues.name;\
} while (false)

#define SHINY_ASSIGN_BUTTON(name) do {\
inputValues.name = buttonValue(buttons, buttonCount, map->name);\
newButtonInputValues.name = inputValues.name && !lastInputValues.name;\
} while (false)

void Controller::poll() {
   updateMap();

   int buttonCount, axisCount;
   const unsigned char* buttons = glfwGetJoystickButtons(controllerNum, &buttonCount);
   const float* axes = glfwGetJoystickAxes(controllerNum, &axisCount);

   lastInputValues = inputValues;

   // If there is no physical controller, set the default values
   if (!buttons || !axes || !map->name) {
      inputValues = { 0 };
      newButtonInputValues = inputValues;
      return;
   }

   // Axis values
   SHINY_ASSIGN_AXIS(leftThumbHorizontal);
   SHINY_ASSIGN_AXIS(leftThumbVertical);

   SHINY_ASSIGN_AXIS(rightThumbHorizontal);
   SHINY_ASSIGN_AXIS(rightThumbVertical);

   SHINY_ASSIGN_AXIS(leftTriggerAxis);
   SHINY_ASSIGN_AXIS(rightTriggerAxis);

   // Button values
   SHINY_ASSIGN_BUTTON(dPadLeft);
   SHINY_ASSIGN_BUTTON(dPadRight);
   SHINY_ASSIGN_BUTTON(dPadUp);
   SHINY_ASSIGN_BUTTON(dPadDown);

   SHINY_ASSIGN_BUTTON(faceLeft);
   SHINY_ASSIGN_BUTTON(faceRight);
   SHINY_ASSIGN_BUTTON(faceUp);
   SHINY_ASSIGN_BUTTON(faceDown);

   SHINY_ASSIGN_BUTTON(leftTrigger);
   SHINY_ASSIGN_BUTTON(rightTrigger);

   SHINY_ASSIGN_BUTTON(leftBumper);
   SHINY_ASSIGN_BUTTON(rightBumper);

   SHINY_ASSIGN_BUTTON(leftThumb);
   SHINY_ASSIGN_BUTTON(rightThumb);

   SHINY_ASSIGN_BUTTON(leftMenu);
   SHINY_ASSIGN_BUTTON(rightMenu);

   handleSpecialCases(buttons, axes, buttonCount, axisCount);
}

bool Controller::connected() const {
   return glfwJoystickPresent(controllerNum) == GL_TRUE;
}

const ControllerValues& Controller::values(bool onlyNewButtons) const {
   return onlyNewButtons ? newButtonInputValues : inputValues;
}

} // namespace Shiny
