#ifndef SHINY_AUDIO_ERROR_H
#define SHINY_AUDIO_ERROR_H

#include "Shiny/ShinyAssert.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace Shiny {

namespace {

const char* alErrorString(ALenum error) {
   switch (error) {
      case AL_NO_ERROR:
         return "no error";
      case AL_INVALID_NAME:
         return "invalid name parameter";
      case AL_INVALID_ENUM:
         return "invalid enum parameter";
      case AL_INVALID_VALUE:
         return "invalid value parameter";
      case AL_INVALID_OPERATION:
         return "illegal operation";
      case AL_OUT_OF_MEMORY:
         return "out of memory";
      default:
         return "invalid error";
   }
}

const char* alcErrorString(ALCenum error) {
   switch (error) {
      case ALC_NO_ERROR:
         return "no error";
      case ALC_INVALID_DEVICE:
         return "invalid device parameter";
      case ALC_INVALID_CONTEXT:
         return "invalid context parameter";
      case ALC_INVALID_ENUM:
         return "invalid enum parameter";
      case ALC_INVALID_VALUE:
         return "invalid value parameter";
      case ALC_OUT_OF_MEMORY:
         return "out of memory";
      default:
         return "invalid error";
   }
}

} // namespace

} // namespace Shiny

#if SHINY_DEBUG
#  define SHINY_CHECK_AL_ERROR(location) do{\
ALenum alError = alGetError();\
ASSERT(alError == AL_NO_ERROR, "OpenAL error while %s: %s", (location), Shiny::alErrorString(alError));\
}while(0)
#else // SHINY_DEBUG
#  define SHINY_CHECK_AL_ERROR(location) do{}while(0)
#endif // SHINY_DEBUG

#if SHINY_DEBUG
#  define SHINY_CHECK_ALC_ERROR(device, location) do{\
ALCenum alcError = alcGetError(device);\
ASSERT(alcError == ALC_NO_ERROR, "OpenAL context error while %s: %s", (location), Shiny::alcErrorString(alcError));\
}while(0)
#else // SHINY_DEBUG
#  define SHINY_CHECK_ALC_ERROR(device, location) do{}while(0)
#endif // SHINY_DEBUG

#define SHINY_CHECK_AUDIO_VALID(check, what, component) ASSERT((check), "Audio error: %s of invalid %s", (what), (component))

#endif
