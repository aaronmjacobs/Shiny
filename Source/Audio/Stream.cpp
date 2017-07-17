 #include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioSystem.h"
#include "Shiny/Audio/Stream.h"

namespace Shiny {
   
Stream::Stream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource)
   : Sound(source), dataSource(std::move(dataSource)) {
   ASSERT(source->getNumBuffersQueued() > 0 || source->getNumBuffersProcessed() > 0,
          "Trying to create stream with audio source that has no buffers");
}

Stream::Stream(Stream &&other)
   : Sound(std::move(other)) {
   move(std::move(other));
}

Stream& Stream::operator=(Stream &&other) {
   Sound::operator=(std::move(other));
   move(std::move(other));
   return *this;
}

void Stream::move(Stream &&other) {
   dataSource = std::move(other.dataSource);
   targetPlayOffset = other.targetPlayOffset;
   looping = other.looping;

   other.targetPlayOffset = kNoTarget;
   other.looping = false;
}

bool Stream::fillBuffers(const std::vector<SPtr<AudioBuffer>> &buffers) {
   bool dataRead = false;

   for (const SPtr<AudioBuffer> &buffer : buffers) {
      bool readSuccess = dataSource->fill(buffer.get());
      if (!readSuccess && looping) {
         if (!dataSource->seekTo(0)) {
            LOG_WARNING("Unable to rewind stream data source");
         }
         readSuccess = dataSource->fill(buffer.get());
      }

      dataRead = dataRead || readSuccess;
   }

   return dataRead;
}

void Stream::setOffsetInBytes(int byteOffset, bool now) {
   ASSERT(byteOffset >= 0 && static_cast<size_t>(byteOffset) < dataSource->getSize(), "Invalid byte offset: %d", byteOffset);

   if (!now) {
      targetPlayOffset = byteOffset;
      return;
   }

   // Put into the 'stopped' state to force all buffers to become 'processed'
   if (source->getState() == AudioSource::State::kInitial) {
      Sound::play();
   }
   Sound::stop();

   std::vector<SPtr<AudioBuffer>> buffers(source->unqueueBuffers(source->getNumBuffersProcessed()));
   if (!dataSource->seekTo(byteOffset)) {
      LOG_WARNING("Unable to seek stream data source");
   }

   fillBuffers(buffers);
   source->queueBuffers(buffers);
}

void Stream::tick(const float dt) {
   Sound::tick(dt);

   int numProcessed = source->getNumBuffersProcessed();
   if (numProcessed == 0) {
      return;
   }

   std::vector<SPtr<AudioBuffer>> buffers(source->unqueueBuffers(numProcessed));
   bool dataRead = fillBuffers(buffers);
   source->queueBuffers(buffers);

   // The only time the source should ever be in the 'stopped' state is if OpenAL is starved for data
   // (the Stream 'stop' function actually puts the source in the 'initial' state)
   // If OpenAL stopped due to data starvation, but we provided more data, start playing again
   if (dataRead && source->getState() == AudioSource::State::kStopped) {
      // This is bat country!
      Sound::play();
   }
}

void Stream::play() {
   if (targetPlayOffset != kNoTarget || !isPaused()) {
      setOffsetInBytes(targetPlayOffset == kNoTarget ? 0 : targetPlayOffset, true);
   }
   Sound::play();

   targetPlayOffset = kNoTarget;
}

void Stream::stop() {
   rewind();
}

void Stream::rewind() {
   Sound::rewind();

   targetPlayOffset = kNoTarget;
}

float Stream::getOffsetInSeconds() const {
   return AudioBuffer::samplesToSeconds(getOffsetInSamples(), dataSource->getFrequency());
}

void Stream::setOffsetInSeconds(float secondOffset) {
   setOffsetInSamples(AudioBuffer::secondsToSamples(secondOffset, dataSource->getFrequency()));
}

int Stream::getOffsetInSamples() const {
   return AudioBuffer::bytesToSamples(getOffsetInBytes(), dataSource->getNumChannels(), dataSource->getBitsPerSample());
}

void Stream::setOffsetInSamples(int sampleOffset) {
   setOffsetInBytes(AudioBuffer::samplesToBytes(sampleOffset,
                                                dataSource->getNumChannels(), dataSource->getBitsPerSample()));
}

int Stream::getOffsetInBytes() const {
   if (source->getState() == AudioSource::State::kInitial) {
      return 0;
   }

   const std::list<SPtr<AudioBuffer>> &queuedBuffers = source->getBufferQueue();
   int activeSize = 0;
   for (const SPtr<AudioBuffer> &buffer : queuedBuffers) {
      activeSize += buffer->getSize();
   }

   int offset = (static_cast<int>(dataSource->getOffset()) - activeSize) + Sound::getOffsetInBytes();
   // Offset can be less than zero when looping (due to data source seeking to beginnig while stream is playing)
   if (offset < 0) {
      int dataSize = static_cast<int>(dataSource->getSize());
      ASSERT(dataSize != 0, "Data size is zero, but offset is not: %d", offset);

      int numDataSteps = 1 + ((-offset - 1) / dataSize);
      offset += dataSize * numDataSteps;
   }

   return offset;
}

void Stream::setOffsetInBytes(int byteOffset) {
   bool playing = isPlaying();
   setOffsetInBytes(byteOffset, playing);

   if (playing) {
      Sound::play();
   }
}

float Stream::getSizeInSeconds() const {
   return AudioBuffer::samplesToSeconds(getSizeInSamples(), dataSource->getFrequency());
}

int Stream::getSizeInSamples() const {
   return AudioBuffer::bytesToSamples(getSizeInBytes(), dataSource->getNumChannels(), dataSource->getBitsPerSample());
}

int Stream::getSizeInBytes() const {
   return static_cast<int>(dataSource->getSize());
}

} // namespace Shiny
