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
       * if more than zero bytes are placed in the buffer, false otherwise.
       */
      virtual bool fill(AudioBuffer *buffer) = 0;

      virtual bool seekTo(int offset) = 0;

      virtual int getOffset() const = 0;

      virtual int getSize() const = 0;

      virtual int getNumChannels() const = 0;

      virtual int getBitsPerSample() const = 0;

      virtual int getFrequency() const = 0;
   };

class SHINYAPI Stream : public Sound {
protected:
   UPtr<StreamDataSource> dataSource;
   int targetPlayOffset;
   bool looping;

   Stream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource);

   bool fillBuffers(const std::vector<SPtr<AudioBuffer>> &buffers);

   void setOffsetInBytes(int byteOffset, bool now);

public:
   friend SPtr<Stream> AudioSystem::generateStream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource);

   virtual ~Stream() override;

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
