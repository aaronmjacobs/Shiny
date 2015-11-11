#include "Shiny/ShinyAssert.h"

#include "Shiny/Audio/Sound.h"

namespace Shiny {
   
Sound::Sound(const SPtr<AudioSource> &source)
   : source(source), fadeInfo({}) {
   ASSERT(source, "Trying to create sound with null source");
}

Sound::~Sound() {
}

void Sound::tick(const float dt) {
   // Fading
   if (fadeInfo.progress < fadeInfo.time) {
      fadeInfo.progress = glm::min(fadeInfo.progress + dt, fadeInfo.time);

      float amount = fadeInfo.time == 0.0f ? 1.0f : fadeInfo.progress / fadeInfo.time;
      float gain = glm::smoothstep(fadeInfo.initialGain, fadeInfo.finalGain, amount);
      setGain(gain);

      if (fadeInfo.progress >= fadeInfo.time) {
         // Finished the fade
         switch (fadeInfo.action) {
            case FadeAction::kNothing:
               break;
            case FadeAction::kPause:
               pause();
               break;
            case FadeAction::kStop:
               stop();
               break;
         }
      }
   }
}

void Sound::fade(float time, float finalGain, FadeAction action) {
   ASSERT(time > 0.0f, "Invalid fade time: %f", time);

   fadeInfo.time = time;
   fadeInfo.progress = 0.0f;
   fadeInfo.initialGain = getGain();
   fadeInfo.finalGain = finalGain;
   fadeInfo.action = action;
}

} // namespace Shiny
