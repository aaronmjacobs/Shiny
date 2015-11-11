#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioSystem.h"
#include "Shiny/Audio/Stream.h"

namespace Shiny {

namespace {

const int kNumBuffers = 2;

} // namespace
   
Stream::Stream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource)
   : Sound(source), dataSource(std::move(dataSource)), initialized(false), looping(false), allowedToStop(false) {
   ASSERT(source->getNumBuffersQueued() == 0 && source->getNumBuffersProcessed() == 0,
          "Trying to create stream with audio source that already has buffers");
}

Stream::~Stream() {
}

void Stream::fillBuffers(const std::vector<SPtr<AudioBuffer>> &buffers) {
   for (const SPtr<AudioBuffer> &buffer : buffers) {
      if (!dataSource->fill(buffer.get(), looping)) {
         // No more data to read
         allowedToStop = true;
      }
   }
}

void Stream::initialize(const AudioSystem &audioSystem) {
   ASSERT(!initialized, "Trying to initialize already-initialized stream");

   if (!dataSource->seekTo(0)) {
      LOG_WARNING("Unable to rewind stream data source during initialization");
   }

   std::vector<SPtr<AudioBuffer>> buffers(kNumBuffers);
   for (SPtr<AudioBuffer> &buffer : buffers) {
      buffer = audioSystem.generateBuffer();
   }

   fillBuffers(buffers);
   source->queueBuffers(buffers);

   initialized = true;
}

void Stream::tick(const float dt) {
   ASSERT(initialized, "Trying to tick uninitialized stream");

   Sound::tick(dt);

   int numProcessed = source->getNumBuffersProcessed();
   if (numProcessed == 0) {
      return;
   }

   std::vector<SPtr<AudioBuffer>> buffers(source->unqueueBuffers(numProcessed));

   fillBuffers(buffers);
   source->queueBuffers(buffers);

   if (!allowedToStop && source->getState() == AudioSource::State::kStopped) {
      // This is bat country!
      play();
   }
}

void Stream::play() {
   ASSERT(initialized, "Trying to play uninitialized stream");

   AudioSource::State state = source->getState();
   if (state == AudioSource::State::kStopped || state == AudioSource::State::kPlaying) {
      rewind();
   }

   Sound::play();
   allowedToStop = false;
}

void Stream::stop() {
   ASSERT(initialized, "Trying to stop uninitialized stream");

   Sound::stop();
   setOffsetInBytes(0);
   allowedToStop = true;
}

void Stream::rewind() {
   ASSERT(initialized, "Trying to rewind uninitialized stream");

   stop();
   Sound::rewind();
}

float Stream::getOffsetInSeconds() const {
   ASSERT(initialized, "Trying to get second offset of uninitialized stream");

   return AudioBuffer::samplesToSeconds(getOffsetInSamples(), dataSource->getFrequency());
}

void Stream::setOffsetInSeconds(float secondOffset) {
   ASSERT(initialized, "Trying to set second offset of uninitialized stream");

   setOffsetInSamples(AudioBuffer::secondsToSamples(secondOffset, dataSource->getFrequency()));
}

int Stream::getOffsetInSamples() const {
   ASSERT(initialized, "Trying to get sample offset of uninitialized stream");

   return AudioBuffer::bytesToSamples(getOffsetInBytes(), dataSource->getNumChannels(), dataSource->getBitsPerSample());
}

void Stream::setOffsetInSamples(int sampleOffset) {
   ASSERT(initialized, "Trying to set sample offset of uninitialized stream");

   setOffsetInBytes(AudioBuffer::samplesToBytes(sampleOffset,
                                                dataSource->getNumChannels(), dataSource->getBitsPerSample()));
}

int Stream::getOffsetInBytes() const {
   ASSERT(initialized, "Trying to get byte offset of uninitialized stream");

   const std::list<SPtr<AudioBuffer>> &queuedBuffers = source->getBufferQueue();
   int activeSize = 0;
   for (const SPtr<AudioBuffer> &buffer : queuedBuffers) {
      activeSize += buffer->getSize();
   }

   int offset = (dataSource->getOffset() - activeSize) + Sound::getOffsetInBytes();
   if (offset < 0) {
      // Necessary due to looping (data in buffers, but data source just rewound)
      offset += dataSource->getSize();
   }

   return offset;
}

void Stream::setOffsetInBytes(int byteOffset) {
   ASSERT(initialized, "Trying to set byte offset of uninitialized stream");
   ASSERT(byteOffset >= 0 && byteOffset < dataSource->getSize(), "Invalid byte offset: %d", byteOffset);

   AudioSource::State state = source->getState();

   // Put into the 'stopped' state to force all buffers to become 'processed'
   if (state == AudioSource::State::kInitial) {
      Sound::play();
   }
   Sound::stop();

   std::vector<SPtr<AudioBuffer>> buffers(source->unqueueBuffers(source->getNumBuffersProcessed()));
   if (!dataSource->seekTo(byteOffset)) {
      LOG_WARNING("Unable to seek stream data source");
   }

   fillBuffers(buffers);
   source->queueBuffers(buffers);

   if (state == AudioSource::State::kStopped && byteOffset == 0) {
      return;
   }
   Sound::rewind();
   if (state == AudioSource::State::kInitial) {
      return;
   }
   Sound::play();
   if (state == AudioSource::State::kPlaying) {
      return;
   }
   Sound::pause();
}

float Stream::getSizeInSeconds() const {
   ASSERT(initialized, "Trying to get size in seconds of uninitialized stream");

   return AudioBuffer::samplesToSeconds(getSizeInSamples(), dataSource->getFrequency());
}

int Stream::getSizeInSamples() const {
   ASSERT(initialized, "Trying to get size in samples of uninitialized stream");

   return AudioBuffer::bytesToSamples(getSizeInBytes(), dataSource->getNumChannels(), dataSource->getBitsPerSample());
}

int Stream::getSizeInBytes() const {
   ASSERT(initialized, "Trying to get size in bytes of uninitialized stream");

   return dataSource->getSize();
}

} // namespace Shiny
