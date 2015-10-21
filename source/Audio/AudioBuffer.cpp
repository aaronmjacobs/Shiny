#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioError.h"

#include <AL/al.h>

#define SHINY_CHECK_AUDIO_BUFFER_VALID(what) SHINY_CHECK_AUDIO_VALID(name && alIsBuffer(name) == AL_TRUE, what, "audio buffer")

#define SHINY_CHECK_AL_BUFFER_ERROR(what) SHINY_CHECK_AL_ERROR(what " of audio buffer")

namespace Shiny {

namespace {

ALenum toAlFormat(AudioBuffer::Format format) {
   switch (format) {
      case AudioBuffer::Format::kMono8:
         return AL_FORMAT_MONO8;
      case AudioBuffer::Format::kMono16:
         return AL_FORMAT_MONO16;
      case AudioBuffer::Format::kStereo8:
         return AL_FORMAT_STEREO8;
      case AudioBuffer::Format::kStereo16:
         return AL_FORMAT_STEREO16;
      default:
         ASSERT(false, "Invalid audio format");
         return AL_FORMAT_MONO8;
   }
}

} // namespace

AudioBuffer::AudioBuffer()
   : name(0) {
   alGenBuffers(1, &name);
   SHINY_CHECK_AL_BUFFER_ERROR("generating name");
}

AudioBuffer::~AudioBuffer() {
   alDeleteBuffers(1, &name);
   SHINY_CHECK_AL_BUFFER_ERROR("deleting name");
}

void AudioBuffer::setData(Format format, const void *data, int size, int frequency) {
   SHINY_CHECK_AUDIO_BUFFER_VALID("setting data");
   ASSERT(size >= 0, "Trying to set negative size for audio buffer");
   ASSERT(frequency >= 0, "Trying to set negative frequency for audio buffer");

   alBufferData(name, toAlFormat(format), data, size, frequency);
   SHINY_CHECK_AL_BUFFER_ERROR("setting data");
}

unsigned int AudioBuffer::getName() const {
   SHINY_CHECK_AUDIO_BUFFER_VALID("getting name");

   return name;
}

int AudioBuffer::getFrequency() const {
   SHINY_CHECK_AUDIO_BUFFER_VALID("getting frequency");

   int frequency = 0;
   alGetBufferi(name, AL_FREQUENCY, &frequency);
   SHINY_CHECK_AL_BUFFER_ERROR("getting frequency");

   return frequency;
}

int AudioBuffer::getSize() const {
   SHINY_CHECK_AUDIO_BUFFER_VALID("getting size");

   int size = 0;
   alGetBufferi(name, AL_SIZE, &size);
   SHINY_CHECK_AL_BUFFER_ERROR("getting size");

   return size;
}

int AudioBuffer::getBitsPerSample() const {
   SHINY_CHECK_AUDIO_BUFFER_VALID("getting bits per sample");

   int bitsPerSample = 0;
   alGetBufferi(name, AL_BITS, &bitsPerSample);
   SHINY_CHECK_AL_BUFFER_ERROR("getting bits per sample");

   return bitsPerSample;
}

int AudioBuffer::getNumChannels() const {
   SHINY_CHECK_AUDIO_BUFFER_VALID("getting number of channels");

   int numSamples = 0;
   alGetBufferi(name, AL_CHANNELS, &numSamples);
   SHINY_CHECK_AL_BUFFER_ERROR("getting number of channels");

   return numSamples;
}

int AudioBuffer::getLengthInSamples() const {
   SHINY_CHECK_AUDIO_BUFFER_VALID("getting length in samples");

   int sizeInBytes = getSize();
   int numChannels = getNumChannels();
   int bitsPerSample = getBitsPerSample();

   return (sizeInBytes * 8) / (numChannels * bitsPerSample);
}

float AudioBuffer::getLengthInSeconds() const {
   SHINY_CHECK_AUDIO_BUFFER_VALID("getting length in seconds");

   int lengthInSamples = getLengthInSamples();
   int frequency = getFrequency();

   return static_cast<float>(lengthInSamples) / frequency;
}

} // namespace Shiny
