#ifndef SHINY_AUDIO_BUFFER_H
#define SHINY_AUDIO_BUFFER_H

#include "Shiny/Pointers.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/AudioSystem.h"

namespace Shiny {
   
class AudioBuffer {
public:
   enum class Format {
      kMono8,
      kMono16,
      kStereo8,
      kStereo16
   };

   static int bytesToSamples(int numBytes, int numChannels, int bitsPerSample) {
      ASSERT(numChannels > 0 && bitsPerSample > 0,
          "Invalid number of channels / bits per sample: %d / %d", numChannels, bitsPerSample);
      ASSERT(bitsPerSample % 8 == 0, "Bits per sample should be evenly divisble by 8: %d", bitsPerSample);

      return (numBytes * 8) / (numChannels * bitsPerSample);
   }

   static int samplesToBytes(int numSamples, int numChannels, int bitsPerSample) {
      ASSERT(numChannels > 0 && bitsPerSample > 0,
          "Invalid number of channels / bits per sample: %d / %d", numChannels, bitsPerSample);
      ASSERT(bitsPerSample % 8 == 0, "Bits per sample should be evenly divisble by 8: %d", bitsPerSample);

      return numSamples * numChannels * (bitsPerSample / 8);
   }

   static float samplesToSeconds(int numSamples, int frequency) {
      ASSERT(frequency > 0, "Invalid frequency: %d", frequency);

      return static_cast<float>(numSamples) / frequency;
   }

   static int secondsToSamples(float numSeconds, int frequency) {
      ASSERT(frequency > 0, "Invalid frequency: %d", frequency);

      return static_cast<int>(numSeconds * frequency);
   }

protected:
   unsigned int name { 0 };

   AudioBuffer();

   void release();

   void move(AudioBuffer &&other);

public:
   friend SPtr<AudioBuffer> AudioSystem::generateBuffer() const;

   AudioBuffer(AudioBuffer &&other);

   AudioBuffer& operator=(AudioBuffer &&other);

   ~AudioBuffer();

   void setData(Format format, const void *data, int size, int frequency);

   unsigned int getName() const;

   int getFrequency() const;

   int getSize() const;

   int getBitsPerSample() const;

   int getNumChannels() const;

   int getSizeInSamples() const;

   float getSizeInSeconds() const;
};

} // namespace Shiny

#endif
