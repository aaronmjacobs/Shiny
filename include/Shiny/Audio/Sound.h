#ifndef SHINY_SOUND_H
#define SHINY_SOUND_H

#include "Shiny/Pointers.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioSource.h"
#include "Shiny/Audio/AudioSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Shiny {

class AudioSource;

class Sound {
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
   FadeInfo fadeInfo {};

   Sound(const SPtr<AudioSource> &source);

   void move(Sound &&other);

public:
   friend SPtr<Sound> AudioSystem::generateSound(const SPtr<AudioSource> &source);

   Sound(Sound &&other);

   Sound& operator=(Sound &&other);

   virtual ~Sound() = default;

   virtual void tick(const float dt);

   void fade(float time, float finalGain, FadeAction action);

   // Passthrough functions

   virtual void play() {
      ASSERT(source);
      source->play();
   }

   void pause() {
      ASSERT(source);
      source->pause();
   }

   virtual void stop() {
      ASSERT(source);
      source->stop();
   }

   virtual void rewind() {
      ASSERT(source);
      source->rewind();
   }

   bool isPlaying() const {
      ASSERT(source);
      return source->getState() == AudioSource::State::kPlaying;
   }

   bool isPaused() const {
      ASSERT(source);
      return source->getState() == AudioSource::State::kPaused;
   }

   bool isRelative() const {
      ASSERT(source);
      return source->isRelative();
   }

   void setRelative(bool relative) {
      ASSERT(source);
      source->setRelative(relative);
   }

   virtual bool isLooping() const {
      ASSERT(source);
      return source->isLooping();
   }

   virtual void setLooping(bool looping) {
      ASSERT(source);
      source->setLooping(looping);
   }

   float getPitch() const {
      ASSERT(source);
      return source->getPitch();
   }

   void setPitch(float pitch) {
      ASSERT(source);
      source->setPitch(pitch);
   }

   float getGain() const {
      ASSERT(source);
      return source->getGain();
   }

   void setGain(float gain) {
      ASSERT(source);
      source->setGain(gain);
   }

   glm::vec3 getPosition() const {
      ASSERT(source);
      return source->getPosition();
   }

   void setPosition(const glm::vec3 &position) {
      ASSERT(source);
      source->setPosition(position);
   }

   glm::vec3 getVelocity() const {
      ASSERT(source);
      return source->getVelocity();
   }

   void setVelocity(const glm::vec3 &velocity) {
      ASSERT(source);
      source->setVelocity(velocity);
   }

   glm::vec3 getDirection() const {
      ASSERT(source);
      return source->getDirection();
   }

   void setDirection(const glm::vec3 &direction) {
      ASSERT(source);
      source->setDirection(direction);
   }

   float getConeInnerAngle() const {
      ASSERT(source);
      return source->getConeInnerAngle();
   }

   void setConeInnerAngle(float coneInnerAngle) {
      ASSERT(source);
      source->setConeInnerAngle(coneInnerAngle);
   }

   float getConeOuterAngle() const {
      ASSERT(source);
      return source->getConeOuterAngle();
   }

   void setConeOuterAngle(float coneOuterAngle) {
      ASSERT(source);
      source->setConeOuterAngle(coneOuterAngle);
   }

   float getConeOuterGain() const {
      ASSERT(source);
      return source->getConeOuterGain();
   }

   void setConeOuterGain(float coneOuterGain) {
      ASSERT(source);
      source->setConeOuterGain(coneOuterGain);
   }

   virtual float getOffsetInSeconds() const {
      ASSERT(source);
      return source->getOffsetInSeconds();
   }

   virtual void setOffsetInSeconds(float secondOffset) {
      ASSERT(source);
      source->setOffsetInSeconds(secondOffset);
   }

   virtual int getOffsetInSamples() const {
      ASSERT(source);
      return source->getOffsetInSamples();
   }

   virtual void setOffsetInSamples(int sampleOffset) {
      ASSERT(source);
      source->setOffsetInSamples(sampleOffset);
   }

   virtual int getOffsetInBytes() const {
      ASSERT(source);
      return source->getOffsetInBytes();
   }

   virtual void setOffsetInBytes(int byteOffset) {
      ASSERT(source);
      source->setOffsetInBytes(byteOffset);
   }

   virtual float getSizeInSeconds() const {
      ASSERT(source);
      const SPtr<AudioBuffer> &buffer = source->getCurrentBuffer();
      return buffer ? buffer->getSizeInSeconds() : 0.0f;
   }

   virtual int getSizeInSamples() const {
      ASSERT(source);
      const SPtr<AudioBuffer> &buffer = source->getCurrentBuffer();
      return buffer ? buffer->getSizeInSamples() : 0;
   }

   virtual int getSizeInBytes() const {
      ASSERT(source);
      const SPtr<AudioBuffer> &buffer = source->getCurrentBuffer();
      return buffer ? buffer->getSize() : 0;
   }
};

} // namespace Shiny

#endif
