#ifndef SHINY_INPUT_DEVICE_H
#define SHINY_INPUT_DEVICE_H

namespace Shiny {

class InputDevice {
public:
   virtual ~InputDevice() = default;

   virtual void poll() = 0;
};

} // namespace Shiny

#endif
