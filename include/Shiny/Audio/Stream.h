#ifndef SHINY_STREAM_H
#define SHINY_STREAM_H

#include "Shiny/Defines.h"

#include "Shiny/Audio/Sound.h"

#include <vector>

namespace Shiny {

class AudioBuffer;

class SHINYAPI StreamDataSource {
   public:
      /*!
       * Fills the buffer with audio data. If no more data is available, sets the buffer data to be empty. Returns true
       * if buffer is able to be filled entirely, false otherwise.
       */
      virtual bool fill(AudioBuffer *buffer, bool circular) = 0;

      virtual bool seekTo(int offset) = 0;

      virtual int getOffset() = 0;

      virtual int getSize() = 0;

      virtual int getNumChannels() = 0;

      virtual int getBitsPerSample() = 0;

      virtual int getFrequency() = 0;
   };

class SHINYAPI Stream : public Sound {
protected:
   UPtr<StreamDataSource> dataSource;
   bool initialized;
   bool looping;
   bool allowedToStop;

   Stream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource);

   void fillBuffers(const std::vector<SPtr<AudioBuffer>> &buffers);

public:
   friend SPtr<Stream> AudioSystem::generateStream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource);

   virtual ~Stream() override;

   void initialize(const AudioSystem &audioSystem);

   virtual void tick(const float dt) override;

   virtual void play() override;

   virtual void stop() override;

   virtual void rewind() override;

   virtual bool isLooping() const override {
      return looping;
   }

   virtual void setLooping(bool looping) override {
      this->looping = looping;
   }

   virtual float getOffsetInSeconds() const override;

   virtual void setOffsetInSeconds(float secondOffset) override;

   virtual int getOffsetInSamples() const override;

   virtual void setOffsetInSamples(int sampleOffset) override;

   virtual int getOffsetInBytes() const override;

   virtual void setOffsetInBytes(int byteOffset) override;

   virtual float getSizeInSeconds() const override;

   virtual int getSizeInSamples() const override;

   virtual int getSizeInBytes() const override;
};

}

#endif
