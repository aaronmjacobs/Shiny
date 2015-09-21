#include "Shiny/Input/Mouse.h"

#include <GLFW/glfw3.h>

namespace Shiny {

Mouse::Mouse(GLFWwindow* const window)
   : window(window), x(0.0), y(0.0), left(false), middle(false), right(false) {
}

Mouse::~Mouse() {
}

void Mouse::poll() {
   glfwGetCursorPos(window, &x, &y);

   left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
   middle = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
   right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

} // namespace Shiny
