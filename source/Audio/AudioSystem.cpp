#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioError.h"
#include "Shiny/Audio/AudioSource.h"
#include "Shiny/Audio/AudioSystem.h"
#include "Shiny/Audio/Sound.h"
#include "Shiny/Audio/Stream.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <array>

#define SHINY_CHECK_AUDIO_SYSTEM_VALID(what) SHINY_CHECK_AUDIO_VALID(context, what, "audio system")

#define SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT(what) do{\
SHINY_CHECK_AUDIO_SYSTEM_VALID(what);\
SHINY_CHECK_AUDIO_VALID(isContextCurrent(), what, "audio system context");\
}while(0)

namespace Shiny {

namespace {

const glm::vec3 kForward(0.0f, 0.0f, -1.0f);
const glm::vec3 kUp(0.0f, 1.0f, 0.0f);

void closeDevice(ALCdevice *device) {
   if (device) {
      alcCloseDevice(device);
      SHINY_CHECK_ALC_ERROR(device, "closing device");
   }
}

SPtr<ALCdevice> getDevice() {
   static WPtr<ALCdevice> wDevice;

   SPtr<ALCdevice> device = wDevice.lock();

   if (!device) {
      device = SPtr<ALCdevice>(alcOpenDevice(nullptr), closeDevice);
      wDevice = device;
   }

   return device;
}

void destroyContext(ALCcontext *context) {
   if (context) {
      ALCcontext *current = alcGetCurrentContext();
      if (context == current) {
         alcMakeContextCurrent(nullptr);
      }

      alcDestroyContext(context);
   }
}

AudioSystem::DistanceModel getDistanceModelClass(ALint alDistanceModel) {
   switch (alDistanceModel) {
      case AL_NONE:
         return AudioSystem::DistanceModel::kNone;
      case AL_INVERSE_DISTANCE:
         return AudioSystem::DistanceModel::kInverseDistance;
      case AL_INVERSE_DISTANCE_CLAMPED:
         return AudioSystem::DistanceModel::kInverseDistanceClamped;
      case AL_LINEAR_DISTANCE:
         return AudioSystem::DistanceModel::kLinearDistance;
      case AL_LINEAR_DISTANCE_CLAMPED:
         return AudioSystem::DistanceModel::kLinearDistanceClamped;
      case AL_EXPONENT_DISTANCE:
         return AudioSystem::DistanceModel::kExponentDistance;
      case AL_EXPONENT_DISTANCE_CLAMPED:
         return AudioSystem::DistanceModel::kExponentDistanceClamped;
      default:
         ASSERT(false, "Invalid AL distance model");
         return AudioSystem::DistanceModel::kNone;
   }
}

ALenum getDistanceModelEnum(AudioSystem::DistanceModel distanceModel) {
   switch (distanceModel) {
      case AudioSystem::DistanceModel::kNone:
         return AL_NONE;
      case AudioSystem::DistanceModel::kInverseDistance:
         return AL_INVERSE_DISTANCE;
      case AudioSystem::DistanceModel::kInverseDistanceClamped:
         return AL_INVERSE_DISTANCE_CLAMPED;
      case AudioSystem::DistanceModel::kLinearDistance:
         return AL_LINEAR_DISTANCE;
      case AudioSystem::DistanceModel::kLinearDistanceClamped:
         return AL_LINEAR_DISTANCE_CLAMPED;
      case AudioSystem::DistanceModel::kExponentDistance:
         return AL_EXPONENT_DISTANCE;
      case AudioSystem::DistanceModel::kExponentDistanceClamped:
         return AL_EXPONENT_DISTANCE_CLAMPED;
      default:
         ASSERT(false, "Invalid distance model class");
         return AL_NONE;
   }
}

} // namespace

// static
const char* AudioSystem::errorString(Result result) {
   switch (result) {
      case Result::kOK:
         return "No error";
      case Result::kDevice:
         return "Unable to open audio device";
      case Result::kContext:
         return "Unable to create audio context";
      case Result::kContextCurrent:
         return "Unable to make audio context current";
      default:
         return "Unknown error";
   }
}

AudioSystem::~AudioSystem() {
   shutDown();
}

AudioSystem::AudioSystem(AudioSystem &&other) {
   move(std::move(other));
}

AudioSystem& AudioSystem::operator=(AudioSystem &&other) {
   shutDown();
   move(std::move(other));
   return *this;
}

void AudioSystem::move(AudioSystem &&other) {
   device = std::move(other.device);
   context = std::move(other.context);
   sounds = std::move(other.sounds);
}

AudioSystem::Result AudioSystem::startUp() {
   ASSERT(!device && !context, "Trying to start up audio system that has already been started");

   device = getDevice();
   if (!device) {
      LOG_ERROR(errorString(Result::kDevice));
      return Result::kDevice;
   }

   context = UPtr<ALCcontext, std::function<void(ALCcontext*)>>(alcCreateContext(device.get(), nullptr),
                                                                destroyContext);
   SHINY_CHECK_ALC_ERROR(device.get(), "creating audio context");
   if (!context) {
      device = nullptr;
      LOG_ERROR(errorString(Result::kContext));
      return Result::kContext;
   }

   return makeContextCurrent();
}

void AudioSystem::shutDown() {
   // Clean up in order
   context = nullptr;
   device = nullptr;
}

void AudioSystem::tick(const float dt) {
   // Remove all deallocated sounds
   sounds.erase(std::remove_if(sounds.begin(), sounds.end(), [](const WPtr<Sound> &sound) {
      return sound.expired();
   }), sounds.end());

   for (const WPtr<Sound> &wSound : sounds) {
      SPtr<Sound> sound = wSound.lock();
      ASSERT(sound, "Sound should not be deallocated");

      sound->tick(dt);
   }
}

SPtr<AudioBuffer> AudioSystem::generateBuffer() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("generating audio buffer");

   // Tied to the device, not the context, so no need to make it current
   return SPtr<AudioBuffer>(new AudioBuffer);
}

SPtr<AudioSource> AudioSystem::generateSource() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("generating audio source");

   return SPtr<AudioSource>(new AudioSource(device.get(), context.get()));
}

SPtr<Sound> AudioSystem::generateSound(const SPtr<AudioSource> &source) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("generating sound");

   SPtr<Sound> sound(new Sound(source));
   sounds.push_back(sound);

   return sound;
}

SPtr<Stream> AudioSystem::generateStream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("generating stream");

   SPtr<Stream> stream(new Stream(source, std::move(dataSource)));
   sounds.push_back(stream);

   return stream;
}

bool AudioSystem::isContextCurrent() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID("checking if audio context is current");

   return alcGetCurrentContext() == context.get();
}

AudioSystem::Result AudioSystem::makeContextCurrent() {
   SHINY_CHECK_AUDIO_SYSTEM_VALID("making audio context current");

   if (isContextCurrent()) {
      return Result::kOK;
   }

   Result result = Result::kOK;
   if (!alcMakeContextCurrent(context.get())) {
      result = Result::kContextCurrent;
      LOG_ERROR(errorString(result));
   }
   SHINY_CHECK_ALC_ERROR(device.get(), "making audio context current");

   return result;
}

AudioSystem::DistanceModel AudioSystem::getDistanceModel() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("getting distance model");

   ALint distanceModel = alGetInteger(AL_DISTANCE_MODEL);
   SHINY_CHECK_AL_ERROR("getting distance model");

   return getDistanceModelClass(distanceModel);
}

void AudioSystem::setDistanceModel(DistanceModel distanceModel) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("setting distance model");

   alDistanceModel(getDistanceModelEnum(distanceModel));
   SHINY_CHECK_AL_ERROR("setting distance model");
}

float AudioSystem::getDopplerFactor() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("getting doppler factor");

   ALfloat dopplerFactor = alGetFloat(AL_DOPPLER_FACTOR);
   SHINY_CHECK_AL_ERROR("getting doppler factor");

   return dopplerFactor;
}

void AudioSystem::setDopplerFactor(float dopplerFactor) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("setting doppler factor");
   ASSERT(dopplerFactor >= 0.0f, "Invalid doppler factor value: %f, should be non-negative", dopplerFactor);

   alDopplerFactor(dopplerFactor);
   SHINY_CHECK_AL_ERROR("setting doppler factor");
}

float AudioSystem::getSpeedOfSound() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("getting speed of sound");

   ALfloat speedOfSound = alGetFloat(AL_SPEED_OF_SOUND);
   SHINY_CHECK_AL_ERROR("getting speed of sound");

   return speedOfSound;
}

void AudioSystem::setSpeedOfSound(float speedOfSound) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("setting speed of sound");
   ASSERT(speedOfSound > 0.0f, "Invalid speed of sound value: %f, sould be positive", speedOfSound);

   alSpeedOfSound(speedOfSound);
   SHINY_CHECK_AL_ERROR("setting speed of sound");
}

glm::vec3 AudioSystem::getListenerPosition() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("getting listener position");

   glm::vec3 position(0.0f);
   alGetListenerfv(AL_POSITION, glm::value_ptr(position));
   SHINY_CHECK_AL_ERROR("getting listener position");

   return position;
}

void AudioSystem::setListenerPosition(const glm::vec3 &position) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("setting listener position");
   ASSERT(!isnan(position.x) && !isnan(position.y) && !isnan(position.z), "Invalid position, has NaN value");

   alListenerfv(AL_POSITION, glm::value_ptr(position));
   SHINY_CHECK_AL_ERROR("setting listener position");
}

glm::vec3 AudioSystem::getListenerVelocity() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("getting listener velocity");

   glm::vec3 velocity(0.0f);
   alGetListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
   SHINY_CHECK_AL_ERROR("getting listener velocity");

   return velocity;
}

void AudioSystem::setListenerVelocity(const glm::vec3 &velocity) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("setting listener velocity");
   ASSERT(!isnan(velocity.x) && !isnan(velocity.y) && !isnan(velocity.z), "Invalid velocity, has NaN value");

   alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
   SHINY_CHECK_AL_ERROR("setting listener velocity");
}

glm::quat AudioSystem::getListenerOrientation() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("getting listener orientation");

   std::array<float, 6> values = {{
      kForward.x, kForward.y, kForward.z,
      kUp.x, kUp.y, kUp.z
   }};
   alGetListenerfv(AL_ORIENTATION, values.data());
   SHINY_CHECK_AL_ERROR("getting listener orientation");

   glm::vec3 direction(values[0], values[1], values[2]);
   glm::vec3 up(values[3], values[4], values[5]);
   return glm::toQuat(glm::lookAtRH(glm::vec3(0.0f), direction, up));
}

void AudioSystem::setListenerOrientation(const glm::quat &orientation) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("setting listener orientation");

   glm::vec3 forward = kForward * orientation;
   glm::vec3 up = kUp * orientation;
   std::array<float, 6> values = {{
      forward.x, forward.y, forward.z,
      up.x, up.y, up.z
   }};

   alListenerfv(AL_ORIENTATION, values.data());
   SHINY_CHECK_AL_ERROR("setting listener orientation");
}

float AudioSystem::getListenerGain() const {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("getting listener gain");

   float gain = 1.0f;
   alGetListenerf(AL_GAIN, &gain);
   SHINY_CHECK_AL_ERROR("getting listener gain");

   return gain;
}

void AudioSystem::setListenerGain(float gain) {
   SHINY_CHECK_AUDIO_SYSTEM_VALID_CURRENT("setting listener gain");
   ASSERT(gain >= 0.0f, "Invalid gain value: %f, sould be non-negative", gain);

   alListenerf(AL_GAIN, gain);
   SHINY_CHECK_AL_ERROR("setting listener gain");
}

} // namespace Shiny
