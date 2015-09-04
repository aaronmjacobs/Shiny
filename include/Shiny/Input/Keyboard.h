#ifndef SHINY_KEYBOARD_H
#define SHINY_KEYBOARD_H

#include "Shiny/Defines.h"

#include "Shiny/Input/InputDevice.h"
#include "Shiny/Input/Keys.h"

#include <array>

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

class SHINYAPI Keyboard : public InputDevice {
protected:
   GLFWwindow* const window;
   std::array<bool, Key::kLast + 1> keys;

public:
   Keyboard(GLFWwindow* const window);

   virtual ~Keyboard();

   virtual void poll() override;

   bool pressed(Key::Code key) const;
};

} // namespace Shiny

#endif
