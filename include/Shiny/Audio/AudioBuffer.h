#ifndef SHINY_AUDIO_BUFFER_H
#define SHINY_AUDIO_BUFFER_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include "Shiny/Audio/AudioSystem.h"

namespace Shiny {
   
class SHINYAPI AudioBuffer {
public:
   enum class SHINYAPI Format {
      kMono8,
      kMono16,
      kStereo8,
      kStereo16
   };

protected:
   unsigned int name;

   AudioBuffer();

public:
   friend SPtr<AudioBuffer> AudioSystem::generateBuffer() const;

   virtual ~AudioBuffer();

   void setData(Format format, const void *data, int size, int frequency);

   unsigned int getName() const;

   int getFrequency() const;

   int getSize() const;

   int getBitsPerSample() const;

   int getNumChannels() const;

   int getLengthInSamples() const;

   float getLengthInSeconds() const;
};

} // namespace Shiny

#endif
