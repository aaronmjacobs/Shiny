#include "Shiny/GLIncludes.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Input/Keyboard.h"

namespace Shiny {

Keyboard::Keyboard(GLFWwindow* const window)
   : window(window) {
   keys.fill(false);
}

Keyboard::~Keyboard() {
}

void Keyboard::poll() {
   for (int i = 0; i < keys.size(); ++i) {
      keys[i] = glfwGetKey(window, i) == GLFW_PRESS;
   }
}

bool Keyboard::pressed(Key::Code key) const {
   ASSERT(key >= 0 && key < keys.size(), "Invalid key code: %d", key);

   return keys[key];
}

} // namespace Shiny
