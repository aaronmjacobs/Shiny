#ifndef SHINY_KEYBOARD_H
#define SHINY_KEYBOARD_H

#include "Shiny/Input/InputDevice.h"
#include "Shiny/Input/Keys.h"

#include <array>

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

class Keyboard : public InputDevice {
protected:
   GLFWwindow* const window;
   std::array<bool, Key::kLast + 1> keys, lastKeys;

public:
   Keyboard(GLFWwindow* const window);

   virtual ~Keyboard();

   virtual void poll() override;

   bool pressed(Key::Code key, bool onlyNew = false) const;
};

} // namespace Shiny

#endif
