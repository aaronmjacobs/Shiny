#ifndef SHINY_SOUND_H
#define SHINY_SOUND_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioSource.h"
#include "Shiny/Audio/AudioSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Shiny {

class AudioSource;

class SHINYAPI Sound {
public:
   enum class FadeAction {
      kNothing,
      kPause,
      kStop
   };

protected:
   struct FadeInfo {
      float time;
      float progress;
      float initialGain;
      float finalGain;
      FadeAction action;
   };

   SPtr<AudioSource> source;
   FadeInfo fadeInfo;

   Sound(const SPtr<AudioSource> &source);

public:
   friend SPtr<Sound> AudioSystem::generateSound(const SPtr<AudioSource> &source);

   virtual ~Sound();

   virtual void tick(const float dt);

   void fade(float time, float finalGain, FadeAction action);

   // Passthrough functions

   virtual void play() {
      source->play();
   }

   void pause() {
      source->pause();
   }

   virtual void stop() {
      source->stop();
   }

   virtual void rewind() {
      source->rewind();
   }

   bool isPlaying() const {
      return source->getState() == AudioSource::State::kPlaying;
   }

   bool isPaused() const {
      return source->getState() == AudioSource::State::kPaused;
   }

   bool isRelative() const {
      return source->isRelative();
   }

   void setRelative(bool relative) {
      source->setRelative(relative);
   }

   virtual bool isLooping() const {
      return source->isLooping();
   }

   virtual void setLooping(bool looping) {
      source->setLooping(looping);
   }

   float getPitch() const {
      return source->getPitch();
   }

   void setPitch(float pitch) {
      source->setPitch(pitch);
   }

   float getGain() const {
      return source->getGain();
   }

   void setGain(float gain) {
      source->setGain(gain);
   }

   glm::vec3 getPosition() const {
      return source->getPosition();
   }

   void setPosition(const glm::vec3 &position) {
      source->setPosition(position);
   }

   glm::vec3 getVelocity() const {
      return source->getVelocity();
   }

   void setVelocity(const glm::vec3 &velocity) {
      source->setVelocity(velocity);
   }

   glm::vec3 getDirection() const {
      return source->getDirection();
   }

   void setDirection(const glm::vec3 &direction) {
      source->setDirection(direction);
   }

   float getConeInnerAngle() const {
      return source->getConeInnerAngle();
   }

   void setConeInnerAngle(float coneInnerAngle) {
      source->setConeInnerAngle(coneInnerAngle);
   }

   float getConeOuterAngle() const {
      return source->getConeOuterAngle();
   }

   void setConeOuterAngle(float coneOuterAngle) {
      source->setConeOuterAngle(coneOuterAngle);
   }

   float getConeOuterGain() const {
      return source->getConeOuterGain();
   }

   void setConeOuterGain(float coneOuterGain) {
      source->setConeOuterGain(coneOuterGain);
   }

   virtual float getOffsetInSeconds() const {
      return source->getOffsetInSeconds();
   }

   virtual void setOffsetInSeconds(float secondOffset) {
      source->setOffsetInSeconds(secondOffset);
   }

   virtual int getOffsetInSamples() const {
      return source->getOffsetInSamples();
   }

   virtual void setOffsetInSamples(int sampleOffset) {
      source->setOffsetInSeconds(sampleOffset);
   }

   virtual int getOffsetInBytes() const {
      return source->getOffsetInBytes();
   }

   virtual void setOffsetInBytes(int byteOffset) {
      source->setOffsetInBytes(byteOffset);
   }

   virtual float getSizeInSeconds() const {
      const SPtr<AudioBuffer> &buffer = source->getCurrentBuffer();
      return buffer ? buffer->getSizeInSeconds() : 0.0f;
   }

   virtual int getSizeInSamples() const {
      const SPtr<AudioBuffer> &buffer = source->getCurrentBuffer();
      return buffer ? buffer->getSizeInSamples() : 0;
   }

   virtual int getSizeInBytes() const {
      const SPtr<AudioBuffer> &buffer = source->getCurrentBuffer();
      return buffer ? buffer->getSize() : 0;
   }
};

} // namespace Shiny

#endif
