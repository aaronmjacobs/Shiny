#ifndef SHINY_AUDIO_LOADER_H
#define SHINY_AUDIO_LOADER_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include <string>

namespace Shiny {

class AudioSource;
class AudioSystem;

class SHINYAPI AudioLoader {
public:
   AudioLoader();

   virtual ~AudioLoader();

   SPtr<AudioSource> loadSound(const AudioSystem &audioSystem, const std::string &fileName);
};

} // namespace Shiny

#endif
