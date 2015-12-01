#include "Shiny/ShinyAssert.h"

#include "Shiny/Input/Keyboard.h"

#include <GLFW/glfw3.h>

namespace Shiny {

Keyboard::Keyboard(GLFWwindow* const window)
   : window(window) {
   keys.fill(false);
   lastKeys.fill(false);

   // Since we only poll (and don't use callbacks), make sure we don't miss any button presses
   glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

   // Set initial state
   poll();
   for (int i = 0; i < keys.size(); ++i) {
      lastKeys[i] = keys[i];
   }
}

Keyboard::~Keyboard() {
}

void Keyboard::poll() {
   for (size_t i = 0; i < keys.size(); ++i) {
      lastKeys[i] = keys[i];
      keys[i] = glfwGetKey(window, i) == GLFW_PRESS;
   }
}

bool Keyboard::pressed(Key::Code key, bool onlyNew) const {
   ASSERT(key >= 0 && static_cast<size_t>(key) < keys.size(), "Invalid key code: %d", key);

   return onlyNew ? keys[key] && !lastKeys[key] : keys[key];
}

} // namespace Shiny
