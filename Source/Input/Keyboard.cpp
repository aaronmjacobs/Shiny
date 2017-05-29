#include "Shiny/ShinyAssert.h"

#include "Shiny/Input/Keyboard.h"

#include <GLFW/glfw3.h>

namespace Shiny {

Keyboard::Keyboard(GLFWwindow* const window)
   : window(window), keys{}, lastKeys{} {
   // Since we only poll (and don't use callbacks), make sure we don't miss any button presses
   glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

   // Set initial state
   poll();
   lastKeys = keys;
}

void Keyboard::poll() {
   lastKeys = keys;
   for (int i = kFirstKey; i <= kLastKey; ++i) {
      keys[i] = glfwGetKey(window, i) == GLFW_PRESS;
   }
}

bool Keyboard::pressed(Key key, bool onlyNew) const {
   int keyIndex = static_cast<int>(key);
   ASSERT(keyIndex >= kFirstKey && keyIndex <= kLastKey, "Invalid key code: %d", keyIndex);

   return onlyNew ? keys[keyIndex] && !lastKeys[keyIndex] : keys[keyIndex];
}

} // namespace Shiny
