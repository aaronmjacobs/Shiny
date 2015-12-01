#ifndef SHINY_INPUT_DEVICE_H
#define SHINY_INPUT_DEVICE_H

namespace Shiny {

class InputDevice {
public:
   InputDevice() {}

   virtual ~InputDevice() {}

   virtual void poll() = 0;
};

} // namespace Shiny

#endif
