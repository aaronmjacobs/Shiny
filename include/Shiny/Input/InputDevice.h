#ifndef SHINY_INPUT_DEVICE_H
#define SHINY_INPUT_DEVICE_H

#include "Shiny/Defines.h"

namespace Shiny {

class SHINYAPI InputDevice {
public:
   InputDevice() {}

   virtual ~InputDevice() {}

   virtual void poll() = 0;
};

} // namespace Shiny

#endif
