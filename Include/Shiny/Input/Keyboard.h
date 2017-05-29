#ifndef SHINY_KEYBOARD_H
#define SHINY_KEYBOARD_H

#include "Shiny/Input/InputDevice.h"
#include "Shiny/Input/Keys.h"

#include <array>

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

namespace Shiny {

class Keyboard : public InputDevice {
public:
   Keyboard(GLFWwindow* const window);

   virtual void poll() override;

   bool pressed(Key key, bool onlyNew = false) const;

private:
   static constexpr int kFirstKey = static_cast<int>(Key::kSpace);
   static constexpr int kLastKey = static_cast<int>(Key::kMenu);

   GLFWwindow* const window;
   std::array<bool, kLastKey + 1> keys, lastKeys;
};

} // namespace Shiny

#endif
