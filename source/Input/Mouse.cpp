#include "Shiny/ShinyAssert.h"

#include "Shiny/Input/Mouse.h"

#include <GLFW/glfw3.h>

namespace Shiny {

namespace {

Mouse::Mode intToMode(int value) {
   switch (value) {
      case GLFW_CURSOR_NORMAL:
         return Mouse::Mode::kNormal;
      case GLFW_CURSOR_HIDDEN:
         return Mouse::Mode::kHidden;
      case GLFW_CURSOR_DISABLED:
         return Mouse::Mode::kDisabled;
      default:
         ASSERT(false, "Invalid input mode int: %d", value);
         return Mouse::Mode::kNormal;
   }
}

int modeToInt(Mouse::Mode mode) {
   switch (mode) {
      case Mouse::Mode::kNormal:
         return GLFW_CURSOR_NORMAL;
      case Mouse::Mode::kHidden:
         return GLFW_CURSOR_HIDDEN;
      case Mouse::Mode::kDisabled:
         return GLFW_CURSOR_DISABLED;
      default:
         ASSERT(false, "Invalid input mode");
         return GLFW_CURSOR_NORMAL;
   }
}

} // namespace

Mouse::Mouse(GLFWwindow* const window)
   : window(window), x(0.0), y(0.0), lastX(0.0), lastY(0.0), left(false), middle(false), right(false),
     lastLeft(false), lastMiddle(false), lastRight(false) {
   // Since we only poll (and don't use callbacks), make sure we don't miss any button presses
   glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

   // Set initial state
   poll();
   lastX = x;
   lastY = y;
   lastLeft = left;
   lastMiddle = middle;
   lastRight = right;
}

Mouse::~Mouse() {
}

void Mouse::poll() {
   lastX = x;
   lastY = y;
   glfwGetCursorPos(window, &x, &y);

   lastLeft = left;
   lastMiddle = middle;
   lastRight = right;
   left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
   middle = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
   right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

Mouse::Mode Mouse::getMode() const {
   return intToMode(glfwGetInputMode(window, GLFW_CURSOR));
}

void Mouse::setMode(Mode mode) const {
   glfwSetInputMode(window, GLFW_CURSOR, modeToInt(mode));
}

} // namespace Shiny
