#ifndef SHINY_AUDIO_SYSTEM_H
#define SHINY_AUDIO_SYSTEM_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <functional>

struct ALCcontext_struct;
typedef struct ALCcontext_struct ALCcontext;
struct ALCdevice_struct;
typedef struct ALCdevice_struct ALCdevice;

namespace Shiny {

class SHINYAPI AudioSystem {
protected:
   SPtr<ALCdevice> device;
   UPtr<ALCcontext, std::function<void(ALCcontext*)>> context;

   void makeContextCurrent();

public:
   AudioSystem();

   virtual ~AudioSystem();

   void startUp();

   void shutDown();

   void setMasterVolume(float volume);

   void setListenerPosition(const glm::vec3 &position);

   void setListenerVelocity(const glm::vec3 &velocity);

   void setListenerOrientation(const glm::quat &orientation);
};

} // namespace Shiny

#endif
