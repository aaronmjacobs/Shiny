#include "Shiny/ShinyAssert.h"

#include "Shiny/Input/Keyboard.h"

#include <GLFW/glfw3.h>

namespace Shiny {

Keyboard::Keyboard(GLFWwindow* const window)
   : window(window) {
   keys.fill(false);
}

Keyboard::~Keyboard() {
}

void Keyboard::poll() {
   for (size_t i = 0; i < keys.size(); ++i) {
      keys[i] = glfwGetKey(window, i) == GLFW_PRESS;
   }
}

bool Keyboard::pressed(Key::Code key) const {
   ASSERT(key >= 0 && static_cast<size_t>(key) < keys.size(), "Invalid key code: %d", key);

   return keys[key];
}

} // namespace Shiny
