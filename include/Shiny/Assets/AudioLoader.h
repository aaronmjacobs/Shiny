#ifndef SHINY_AUDIO_LOADER_H
#define SHINY_AUDIO_LOADER_H

#include "Shiny/Pointers.h"

#include <string>

namespace Shiny {

class AudioSystem;
class Sound;
class Stream;

class AudioLoader {
public:
   SPtr<Sound> loadSound(AudioSystem &audioSystem, const std::string &fileName);

   SPtr<Stream> loadStream(AudioSystem &audioSystem, const std::string &fileName);
};

} // namespace Shiny

#endif
