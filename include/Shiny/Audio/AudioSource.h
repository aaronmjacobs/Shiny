#ifndef SHINY_AUDIO_SOURCE_H
#define SHINY_AUDIO_SOURCE_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include "Shiny/Audio/AudioSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <list>
#include <vector>

struct ALCcontext_struct;
typedef struct ALCcontext_struct ALCcontext;
struct ALCdevice_struct;
typedef struct ALCdevice_struct ALCdevice;

namespace Shiny {

class AudioBuffer;

class SHINYAPI AudioSource {
public:
   enum class SHINYAPI Type {
      kUndetermined,
      kStatic,
      kStreaming
   };

   enum class SHINYAPI State {
      kInitial,
      kPlaying,
      kPaused,
      kStopped
   };

protected:
   unsigned int name;
   ALCdevice* const device;
   ALCcontext* const context;
   std::list<SPtr<AudioBuffer>> bufferQueue;

   AudioSource(ALCdevice* const device, ALCcontext* const context);

public:
   virtual ~AudioSource();

   friend SPtr<AudioSource> AudioSystem::generateSource() const;

   void play();

   void pause();

   void stop();

   void rewind();

   SPtr<AudioBuffer> getCurrentBuffer() const;

   void setCurrentBuffer(const SPtr<AudioBuffer> &buffer);

   void queueBuffers(const std::vector<SPtr<AudioBuffer>> &buffers);

   std::vector<SPtr<AudioBuffer>> unqueueBuffers(int num);

   const std::list<SPtr<AudioBuffer>>& getBufferQueue() const;

   Type getType() const;

   State getState() const;

   bool isRelative() const;

   void setRelative(bool relative);

   bool isLooping() const;

   void setLooping(bool looping);

   int getNumBuffersQueued() const;

   int getNumBuffersProcessed() const;

   float getMinGain() const;

   void setMinGain(float minGain);

   float getMaxGain() const;

   void setMaxGain(float minGain);

   float getReferenceDistance() const;

   void setReferenceDistance(float referenceDistance);

   float getRolloffFactor() const;

   void setRolloffFactor(float rolloffFactor);

   float getMaxDistance() const;

   void setMaxDistance(float maxDistance);

   float getPitch() const;

   void setPitch(float pitch);

   glm::vec3 getPosition() const;

   void setPosition(const glm::vec3 &position);

   glm::vec3 getVelocity() const;

   void setVelocity(const glm::vec3 &velocity);

   glm::vec3 getDirection() const;

   void setDirection(const glm::vec3 &direction);

   float getGain() const;

   void setGain(float gain);

   float getConeInnerAngle() const;

   void setConeInnerAngle(float coneInnerAngle);

   float getConeOuterAngle() const;

   void setConeOuterAngle(float coneOuterAngle);

   float getConeOuterGain() const;

   void setConeOuterGain(float coneOuterGain);

   float getOffsetInSeconds() const;

   void setOffsetInSeconds(float secondOffset);

   int getOffsetInSamples() const;

   void setOffsetInSamples(int sampleOffset);

   int getOffsetInBytes() const;

   void setOffsetInBytes(int byteOffset);
};

} // namespace Shiny

#endif
