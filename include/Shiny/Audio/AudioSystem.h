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

class AudioBuffer;
class AudioSource;

class SHINYAPI AudioSystem {
public:
   enum class SHINYAPI Result {
      kOK,
      kDevice,
      kContext,
      kContextCurrent
   };

   enum class SHINYAPI DistanceModel {
      kNone,
      kInverseDistance,
      kInverseDistanceClamped,
      kLinearDistance,
      kLinearDistanceClamped,
      kExponentDistance,
      kExponentDistanceClamped
   };

   static const char* errorString(Result result);

protected:
   SPtr<ALCdevice> device;
   UPtr<ALCcontext, std::function<void(ALCcontext*)>> context;

public:
   AudioSystem();

   virtual ~AudioSystem();

   Result startUp();

   void shutDown();

   SPtr<AudioBuffer> generateBuffer() const;

   SPtr<AudioSource> generateSource() const;

   bool isContextCurrent() const;

   Result makeContextCurrent();

   DistanceModel getDistanceModel() const;

   void setDistanceModel(DistanceModel distanceModel);

   float getDopplerFactor() const;

   void setDopplerFactor(float dopplerFactor);

   float getSpeedOfSound() const;

   void setSpeedOfSound(float speedOfSound);

   glm::vec3 getListenerPosition() const;

   void setListenerPosition(const glm::vec3 &position);

   glm::vec3 getListenerVelocity() const;

   void setListenerVelocity(const glm::vec3 &velocity);

   glm::quat getListenerOrientation() const;

   void setListenerOrientation(const glm::quat &orientation);

   float getListenerGain() const;

   void setListenerGain(float gain);
};

} // namespace Shiny

#endif
