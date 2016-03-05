#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioError.h"
#include "Shiny/Audio/AudioSource.h"

#include <AL/al.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include <cmath>
#include <limits>

#define SHINY_CHECK_AUDIO_SOURCE_VALID(what) do{\
SHINY_CHECK_AUDIO_VALID(name && alIsSource(name) == AL_TRUE, what, "audio source");\
SHINY_CHECK_AUDIO_VALID(alcGetCurrentContext() == context, what, "audio source (non-current audio context)");\
}while(0)

#define SHINY_CHECK_AL_SOURCE_ERROR(what) SHINY_CHECK_AL_ERROR(what " of audio source")

namespace Shiny {

namespace {

const glm::vec3 kFront(0.0f, 0.0f, -1.0f);
const glm::vec3 kUp(0.0f, 1.0f, 0.0f);

AudioSource::Type getSourceType(ALint alType) {
   switch (alType) {
      case AL_UNDETERMINED:
         return AudioSource::Type::kUndetermined;
      case AL_STATIC:
         return AudioSource::Type::kStatic;
      case AL_STREAMING:
         return AudioSource::Type::kStreaming;
      default:
         ASSERT(false, "Invalid AL source type");
         return AudioSource::Type::kUndetermined;
   }
}

AudioSource::State getSourceState(ALint alState) {
   switch (alState) {
      case AL_INITIAL:
         return AudioSource::State::kInitial;
      case AL_PLAYING:
         return AudioSource::State::kPlaying;
      case AL_PAUSED:
         return AudioSource::State::kPaused;
      case AL_STOPPED:
         return AudioSource::State::kStopped;
      default:
         ASSERT(false, "Invalid AL source state");
         return AudioSource::State::kInitial;
   }
}

bool makeContextCurrent(ALCdevice* const device, ALCcontext* const context) {
   bool success = true;

   if (!alcMakeContextCurrent(context)) {
      success = false;
      LOG_ERROR("Unable to make audio context current inside AudioSource destructor");
   }
   SHINY_CHECK_ALC_ERROR(device, "making audio context current");

   return success;
}

} // namespace

AudioSource::AudioSource(ALCdevice* const device, ALCcontext* const context)
   : device(device), context(context) {
   ASSERT(device, "Trying to create audio source with invalid device");
   ASSERT(context, "Trying to create audio source with invalid context");

   alGenSources(1, &name);
   SHINY_CHECK_AL_SOURCE_ERROR("generating name");
}

AudioSource::AudioSource(AudioSource &&other) {
   move(std::move(other));
}

AudioSource& AudioSource::operator=(AudioSource &&other) {
   release();
   move(std::move(other));
   return *this;
}

AudioSource::~AudioSource() {
   release();
}

void AudioSource::release() {
   if (name == 0) {
      return;
   }

   // Make sure the right context is current
   ALCcontext *currentContext = alcGetCurrentContext();
   if (currentContext != context) {
      if (!makeContextCurrent(device, context)) {
         // If we're unable to make the proper context current, it's probably been destroyed. Not much we can do anyways.
         return;
      }
   }

   alDeleteSources(1, &name);
   SHINY_CHECK_AL_SOURCE_ERROR("deleting name");

   // Set the current context back
   if (currentContext != context) {
      makeContextCurrent(device, currentContext);
   }
}

void AudioSource::move(AudioSource &&other) {
   name = other.name;
   device = other.device;
   context = other.context;
   bufferQueue = std::move(other.bufferQueue);

   other.name = 0;
}

void AudioSource::play() {
   SHINY_CHECK_AUDIO_SOURCE_VALID("playing");

   alSourcePlay(name);
   SHINY_CHECK_AL_SOURCE_ERROR("playing");
}

void AudioSource::pause() {
   SHINY_CHECK_AUDIO_SOURCE_VALID("pausing");

   alSourcePause(name);
   SHINY_CHECK_AL_SOURCE_ERROR("pausing");
}

void AudioSource::stop() {
   SHINY_CHECK_AUDIO_SOURCE_VALID("stopping");

   alSourceStop(name);
   SHINY_CHECK_AL_SOURCE_ERROR("stopping");
}

void AudioSource::rewind() {
   SHINY_CHECK_AUDIO_SOURCE_VALID("rewinding");

   alSourceRewind(name);
   SHINY_CHECK_AL_SOURCE_ERROR("rewinding");
}

SPtr<AudioBuffer> AudioSource::getCurrentBuffer() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting current buffer");

   int bufferName = AL_NONE;
   alGetSourcei(name, AL_BUFFER, &bufferName);
   SHINY_CHECK_AL_SOURCE_ERROR("getting current buffer");

   for (const SPtr<AudioBuffer> &buffer : bufferQueue) {
      if (buffer->getName() == bufferName) {
         return buffer;
      }
   }

   return nullptr;
}

void AudioSource::setCurrentBuffer(const SPtr<AudioBuffer> &buffer) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting current buffer");

   State state = getState();
   ASSERT(state == State::kInitial || state == State::kStopped, "Trying to set current buffer in invalid state");

   bufferQueue = {};
   bufferQueue.push_back(buffer);
   int bufferName = buffer ? (int)buffer->getName() : AL_NONE;

   alSourcei(name, AL_BUFFER, bufferName);
   SHINY_CHECK_AL_SOURCE_ERROR("setting current buffer");
}

void AudioSource::queueBuffers(const std::vector<SPtr<AudioBuffer>> &buffers) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("queueing audio buffers");

#ifndef NDEBUG
   if (!bufferQueue.empty()) {
      const SPtr<AudioBuffer> &frontBuffer = bufferQueue.front();
      int frontFrequency = frontBuffer->getFrequency();
      int frontBitsPerSample = frontBuffer->getBitsPerSample();
      int frontChannels = frontBuffer->getNumChannels();

      for (const SPtr<AudioBuffer> &buffer : buffers) {
         ASSERT(buffer->getFrequency() == frontFrequency, "New buffer frequency doesn't match existing frequency");
         ASSERT(buffer->getBitsPerSample() == frontBitsPerSample, "New buffer bps doesn't match existing bps");
         ASSERT(buffer->getNumChannels() == frontChannels, "New buffer num channels doesn't match existing channels");
      }
   }
#endif

   std::vector<ALuint> bufferNames;
   for (const SPtr<AudioBuffer> &buffer : buffers) {
      bufferQueue.push_back(buffer);
      bufferNames.push_back(buffer->getName());
   }

   alSourceQueueBuffers(name, bufferNames.size(), bufferNames.data());
   SHINY_CHECK_AL_SOURCE_ERROR("queueing audio buffers");
}

std::vector<SPtr<AudioBuffer>> AudioSource::unqueueBuffers(int num) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("unqueueing audio buffers");
   ASSERT(num <= getNumBuffersProcessed(), "Trying to unqueue more buffers than are processed");

   std::vector<ALuint> bufferNames(num);
   alSourceUnqueueBuffers(name, bufferNames.size(), bufferNames.data());
   SHINY_CHECK_AL_SOURCE_ERROR("unqueueing audio buffers");

   std::vector<SPtr<AudioBuffer>> buffers;
   for (ALuint bufferName : bufferNames) {
      ASSERT(!bufferQueue.empty(), "buffer queue size out of sync");

      const SPtr<AudioBuffer> &buffer(bufferQueue.front());
      ASSERT(buffer->getName() == bufferName, "buffer queue order out of sync");
      buffers.push_back(buffer);
      bufferQueue.pop_front();
   }

   return buffers;
}

const std::list<SPtr<AudioBuffer>>& AudioSource::getBufferQueue() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting buffer queue");

   return bufferQueue;
}

AudioSource::Type AudioSource::getType() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting type");

   ALint type = AL_UNDETERMINED;
   alGetSourcei(name, AL_SOURCE_TYPE, &type);
   SHINY_CHECK_AL_SOURCE_ERROR("getting type");

   return getSourceType(type);
}

AudioSource::State AudioSource::getState() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting state");

   ALint state = AL_INITIAL;
   alGetSourcei(name, AL_SOURCE_STATE, &state);
   SHINY_CHECK_AL_SOURCE_ERROR("getting state");

   return getSourceState(state);
}

bool AudioSource::isRelative() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting relative property");

   ALint relative = AL_FALSE;
   alGetSourcei(name, AL_SOURCE_RELATIVE, &relative);
   SHINY_CHECK_AL_SOURCE_ERROR("getting relative property");

   return relative == AL_TRUE;
}

void AudioSource::setRelative(bool relative) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting relative property");

   alSourcei(name, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
   SHINY_CHECK_AL_SOURCE_ERROR("setting relative property");
}

bool AudioSource::isLooping() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting looping property");

   ALint looping = AL_FALSE;
   alGetSourcei(name, AL_LOOPING, &looping);
   SHINY_CHECK_AL_SOURCE_ERROR("getting looping property");

   return looping == AL_TRUE;
}

void AudioSource::setLooping(bool looping) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting looping property");

   alSourcei(name, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
   SHINY_CHECK_AL_SOURCE_ERROR("setting looping property");
}

int AudioSource::getNumBuffersQueued() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting number of buffers queued");

   int numBuffersQueued = 0;
   alGetSourcei(name, AL_BUFFERS_QUEUED, &numBuffersQueued);
   SHINY_CHECK_AL_SOURCE_ERROR("getting number of buffers queued");

   ASSERT(numBuffersQueued == bufferQueue.size(), "Buffer queue size out of sync");
   return numBuffersQueued;
}

int AudioSource::getNumBuffersProcessed() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting number of buffers processed");

   int numBuffersProcessed = 0;
   alGetSourcei(name, AL_BUFFERS_PROCESSED, &numBuffersProcessed);
   SHINY_CHECK_AL_SOURCE_ERROR("getting number of buffers processed");

   return numBuffersProcessed;
}

float AudioSource::getMinGain() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting minimum gain");

   float minGain = 0.0f;
   alGetSourcef(name, AL_MIN_GAIN, &minGain);
   SHINY_CHECK_AL_SOURCE_ERROR("getting minimum gain");

   return minGain;
}

void AudioSource::setMinGain(float minGain) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting minimum gain");
   ASSERT(minGain >= 0.0f && minGain <= 1.0f, "Invalid min gain value: %f, should be in range [0.0f, 1.0f]", minGain);

   alSourcef(name, AL_MIN_GAIN, minGain);
   SHINY_CHECK_AL_SOURCE_ERROR("setting minimum gain");
}

float AudioSource::getMaxGain() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting maximum gain");

   float maxGain = 1.0f;
   alGetSourcef(name, AL_MAX_GAIN, &maxGain);
   SHINY_CHECK_AL_SOURCE_ERROR("getting maximum gain");

   return maxGain;
}

void AudioSource::setMaxGain(float maxGain) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting maximum gain");
   ASSERT(maxGain >= 0.0f && maxGain <= 1.0f, "Invalid max gain value: %f, should be in range [0.0f, 1.0f]", maxGain);

   alSourcef(name, AL_MAX_GAIN, maxGain);
   SHINY_CHECK_AL_SOURCE_ERROR("setting maximum gain");
}

float AudioSource::getReferenceDistance() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting reference distance");

   float referenceDistance = 1.0f;
   alGetSourcef(name, AL_REFERENCE_DISTANCE, &referenceDistance);
   SHINY_CHECK_AL_SOURCE_ERROR("getting reference distance");

   return referenceDistance;
}

void AudioSource::setReferenceDistance(float referenceDistance) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting reference distance");
   ASSERT(referenceDistance >= 0.0f, "Invalid reference distance value: %f, should be non-negative", referenceDistance);

   alSourcef(name, AL_REFERENCE_DISTANCE, referenceDistance);
   SHINY_CHECK_AL_SOURCE_ERROR("setting reference distance");
}

float AudioSource::getRolloffFactor() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting rolloff factor");

   float rolloffFactor = 1.0f;
   alGetSourcef(name, AL_ROLLOFF_FACTOR, &rolloffFactor);
   SHINY_CHECK_AL_SOURCE_ERROR("getting rolloff factor");

   return rolloffFactor;
}

void AudioSource::setRolloffFactor(float rolloffFactor) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting rolloff factor");
   ASSERT(rolloffFactor >= 0.0f, "Invalid rolloff factor value: %f, should be non-negative", rolloffFactor);

   alSourcef(name, AL_ROLLOFF_FACTOR, rolloffFactor);
   SHINY_CHECK_AL_SOURCE_ERROR("setting rolloff factor");
}

float AudioSource::getMaxDistance() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting maximum distance");

   float maxDistance = std::numeric_limits<float>::max();
   alGetSourcef(name, AL_MAX_DISTANCE, &maxDistance);
   SHINY_CHECK_AL_SOURCE_ERROR("getting maximum distance");

   return maxDistance;
}

void AudioSource::setMaxDistance(float maxDistance) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting maximum distance");
   ASSERT(maxDistance >= 0.0f, "Invalid max distance value: %f, should be non-negative", maxDistance);

   alSourcef(name, AL_MAX_DISTANCE, maxDistance);
   SHINY_CHECK_AL_SOURCE_ERROR("setting maximum distance");
}

float AudioSource::getPitch() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting pitch");

   float pitch = 1.0f;
   alGetSourcef(name, AL_PITCH, &pitch);
   SHINY_CHECK_AL_SOURCE_ERROR("getting pitch");

   return pitch;
}

void AudioSource::setPitch(float pitch) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting pitch");
   ASSERT(pitch > 0.0f, "Invalid pitch value: %f, sould be positive", pitch);

   alSourcef(name, AL_PITCH, pitch);
   SHINY_CHECK_AL_SOURCE_ERROR("setting pitch");
}

glm::vec3 AudioSource::getPosition() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting position");

   glm::vec3 position(0.0f);
   alGetSourcefv(name, AL_POSITION, glm::value_ptr(position));
   SHINY_CHECK_AL_SOURCE_ERROR("getting position");

   return position;
}

void AudioSource::setPosition(const glm::vec3 &position) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting position");
   ASSERT(!isnan(position.x) && !isnan(position.y) && !isnan(position.z), "Invalid position, has NaN value");

   alSourcefv(name, AL_POSITION, glm::value_ptr(position));
   SHINY_CHECK_AL_SOURCE_ERROR("setting position");
}

glm::vec3 AudioSource::getVelocity() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting velocity");

   glm::vec3 velocity(0.0f);
   alGetSourcefv(name, AL_VELOCITY, glm::value_ptr(velocity));
   SHINY_CHECK_AL_SOURCE_ERROR("getting velocity");

   return velocity;
}

void AudioSource::setVelocity(const glm::vec3 &velocity) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting velocity");
   ASSERT(!isnan(velocity.x) && !isnan(velocity.y) && !isnan(velocity.z), "Invalid velocity, has NaN value");

   alSourcefv(name, AL_VELOCITY, glm::value_ptr(velocity));
   SHINY_CHECK_AL_SOURCE_ERROR("setting velocity");
}

glm::vec3 AudioSource::getDirection() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting direction");

   glm::vec3 direction(0.0f);
   alGetSourcefv(name, AL_DIRECTION, glm::value_ptr(direction));
   SHINY_CHECK_AL_SOURCE_ERROR("getting direction");

   return direction;
}

void AudioSource::setDirection(const glm::vec3 &direction) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting direction");
   ASSERT(!isnan(direction.x) && !isnan(direction.y) && !isnan(direction.z), "Invalid direction, has NaN value");

   alSourcefv(name, AL_DIRECTION, glm::value_ptr(direction));
   SHINY_CHECK_AL_SOURCE_ERROR("setting direction");
}

float AudioSource::getGain() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting gain");

   float gain = 1.0f;
   alGetSourcef(name, AL_GAIN, &gain);
   SHINY_CHECK_AL_SOURCE_ERROR("getting gain");

   return gain;
}

void AudioSource::setGain(float gain) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting gain");
   ASSERT(gain >= 0.0f, "Invalid gain value: %f, sould be non-negative", gain);

   alSourcef(name, AL_GAIN, gain);
   SHINY_CHECK_AL_SOURCE_ERROR("setting gain");
}

float AudioSource::getConeInnerAngle() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting cone inner angle");

   float coneInnerAngle = 360.0f;
   alGetSourcef(name, AL_CONE_INNER_ANGLE, &coneInnerAngle);
   SHINY_CHECK_AL_SOURCE_ERROR("getting cone inner angle");

   return coneInnerAngle;
}

void AudioSource::setConeInnerAngle(float coneInnerAngle) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting cone inner angle");

   alSourcef(name, AL_CONE_INNER_ANGLE, coneInnerAngle);
   SHINY_CHECK_AL_SOURCE_ERROR("setting cone inner angle");
}

float AudioSource::getConeOuterAngle() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting cone outer angle");

   float coneOuterAngle = 360.0f;
   alGetSourcef(name, AL_CONE_OUTER_ANGLE, &coneOuterAngle);
   SHINY_CHECK_AL_SOURCE_ERROR("getting cone outer angle");

   return coneOuterAngle;
}

void AudioSource::setConeOuterAngle(float coneOuterAngle) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting cone outer angle");

   alSourcef(name, AL_CONE_OUTER_ANGLE, coneOuterAngle);
   SHINY_CHECK_AL_SOURCE_ERROR("setting cone outer angle");
}

float AudioSource::getConeOuterGain() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting cone outer gain");

   float coneOuterGain = 0.0f;
   alGetSourcef(name, AL_CONE_OUTER_GAIN, &coneOuterGain);
   SHINY_CHECK_AL_SOURCE_ERROR("getting cone outer gain");

   return coneOuterGain;
}

void AudioSource::setConeOuterGain(float coneOuterGain) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting cone outer gain");
   ASSERT(coneOuterGain >= 0.0f && coneOuterGain <= 1.0f,
          "Invalid cone outer gain value: %f, should be in range [0.0f, 1.0f]", coneOuterGain);

   alSourcef(name, AL_CONE_OUTER_GAIN, coneOuterGain);
   SHINY_CHECK_AL_SOURCE_ERROR("setting cone outer gain");
}

float AudioSource::getOffsetInSeconds() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting second offset");

   float secondOffset = 0.0f;
   alGetSourcef(name, AL_SEC_OFFSET, &secondOffset);
   SHINY_CHECK_AL_SOURCE_ERROR("getting second offset");

   return secondOffset;
}

void AudioSource::setOffsetInSeconds(float secondOffset) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting offset in seconds");
   ASSERT(secondOffset >= 0.0f, "Invalid second offset value: %f, should be non-negative", secondOffset);

   alSourcef(name, AL_SEC_OFFSET, secondOffset);
   SHINY_CHECK_AL_SOURCE_ERROR("setting offset in seconds");
}

int AudioSource::getOffsetInSamples() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting offset in samples");

   int sampleOffset = 0;
   alGetSourcei(name, AL_SAMPLE_OFFSET, &sampleOffset);
   SHINY_CHECK_AL_SOURCE_ERROR("getting offset in samples");

   return sampleOffset;
}

void AudioSource::setOffsetInSamples(int sampleOffset) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting offset in samples");
   ASSERT(sampleOffset >= 0, "Invalid sample offset value: %d, should be non-negative", sampleOffset);

   alSourcei(name, AL_SAMPLE_OFFSET, sampleOffset);
   SHINY_CHECK_AL_SOURCE_ERROR("setting offset in samples");
}

int AudioSource::getOffsetInBytes() const {
   SHINY_CHECK_AUDIO_SOURCE_VALID("getting offset in bytes");

   int byteOffset = 0;
   alGetSourcei(name, AL_BYTE_OFFSET, &byteOffset);
   SHINY_CHECK_AL_SOURCE_ERROR("getting offset in bytes");

   return byteOffset;
}

void AudioSource::setOffsetInBytes(int byteOffset) {
   SHINY_CHECK_AUDIO_SOURCE_VALID("setting offset in bytes");
   ASSERT(byteOffset >= 0, "Invalid byte offset value: %d, should be non-negative", byteOffset);

   alSourcei(name, AL_BYTE_OFFSET, byteOffset);
   SHINY_CHECK_AL_SOURCE_ERROR("setting offset in bytes");
}

} // namespace Shiny
