#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/AudioSystem.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/gtc/type_ptr.hpp>

#include <array>

namespace Shiny {

namespace {

void closeDevice(ALCdevice *device) {
   if (device) {
      alcCloseDevice(device);
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

const char* errorString(ALCenum error) {
   switch (error) {
      case AL_NO_ERROR:
         return "";
      case AL_INVALID_NAME:
         return "Invalid name parameter";
      case AL_INVALID_ENUM:
         return "Invalid enum parameter";
      case AL_INVALID_VALUE:
         return "Invalid value parameter";
      case AL_INVALID_OPERATION:
         return "Illegal call";
      case AL_OUT_OF_MEMORY:
         return "Unable to allocate memory";
      default:
         return "Invalid error";
   }
}

#ifdef NDEBUG
#define CHECK_AL_ERROR(location) do{}while(0)
#else
#define CHECK_AL_ERROR(location) do{ ALCenum alError = alGetError(); ASSERT(alError == AL_NO_ERROR, "OpenAL error while %s: %s", (location), errorString(alError)); }while(0)
#endif

} // namespace

AudioSystem::AudioSystem() {
}

AudioSystem::~AudioSystem() {
   shutDown();
}

void AudioSystem::makeContextCurrent() {
   ASSERT(context, "Audio context not created");

   ALCcontext *current = alcGetCurrentContext();
   if (context.get() != current) {
      if (!alcMakeContextCurrent(context.get())) {
         LOG_ERROR("Unable to make audio context current");
      }
      CHECK_AL_ERROR("making audio context current");
   }
}

void AudioSystem::startUp() {
   device = getDevice();
   if (!device) {
      LOG_ERROR("Unable to open audio device");
      return;
   }

   context = UPtr<ALCcontext, std::function<void(ALCcontext*)>>(alcCreateContext(device.get(), nullptr),
                                                                destroyContext);
   if (!context) {
      LOG_ERROR("Unable to create audio context");
      return;
   }
}

void AudioSystem::shutDown() {
   // Clean up in order
   context = nullptr;
   device = nullptr;
}

void AudioSystem::setMasterVolume(float volume) {
   makeContextCurrent();

   alListenerf(AL_GAIN, volume);
   CHECK_AL_ERROR("setting master volume");
}

void AudioSystem::setListenerPosition(const glm::vec3 &position) {
   makeContextCurrent();

   alListenerfv(AL_POSITION, glm::value_ptr(position));
   CHECK_AL_ERROR("setting listener position");
}

void AudioSystem::setListenerVelocity(const glm::vec3 &velocity) {
   makeContextCurrent();

   alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
   CHECK_AL_ERROR("setting listener velocity");
}

void AudioSystem::setListenerOrientation(const glm::quat &orientation) {
   static const glm::vec3 kForward(0.0f, 0.0f, -1.0f);
   static const glm::vec3 kUp(0.0f, 1.0f, 0.0f);

   makeContextCurrent();

   glm::vec3 forward = kForward * orientation;
   glm::vec3 up = kUp * orientation;
   std::array<float, 6> values = {{
      forward.x, forward.y, forward.z,
      up.x, up.y, up.z
   }};

   alListenerfv(AL_ORIENTATION, values.data());
   CHECK_AL_ERROR("setting listener orientation");
}

} // namespace Shiny
