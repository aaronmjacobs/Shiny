#ifndef SHINY_AUDIO_LOADER_H
#define SHINY_AUDIO_LOADER_H

#include "Shiny/Pointers.h"
#include "Shiny/Platform/Path.h"

#include <string>

namespace Shiny {

class AudioSystem;
class Sound;
class Stream;

class AudioLoader {
public:
   SPtr<Sound> loadSound(AudioSystem& audioSystem, const Path& filePath);

   SPtr<Stream> loadStream(AudioSystem& audioSystem, const Path& filePath);
};

} // namespace Shiny

#endif
