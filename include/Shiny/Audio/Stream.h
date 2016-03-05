#ifndef SHINY_STREAM_H
#define SHINY_STREAM_H

#include "Shiny/Audio/Sound.h"

#include <vector>

namespace Shiny {

class AudioBuffer;

class StreamDataSource {
   public:
      /*!
       * Fills the buffer with audio data. If no more data is available, sets the buffer data to be empty. Returns true
       * if more than zero bytes are placed in the buffer, false otherwise.
       */
      virtual bool fill(AudioBuffer *buffer) = 0;

      virtual bool seekTo(size_t offset) = 0;

      virtual size_t getOffset() const = 0;

      virtual size_t getSize() const = 0;

      virtual int getNumChannels() const = 0;

      virtual int getBitsPerSample() const = 0;

      virtual int getFrequency() const = 0;
   };

class Stream : public Sound {
private:
   static const int kNoTarget = -1;

protected:
   UPtr<StreamDataSource> dataSource;
   int targetPlayOffset { kNoTarget };
   bool looping { false };

   Stream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource);

   void move(Stream &&other);

   bool fillBuffers(const std::vector<SPtr<AudioBuffer>> &buffers);

   void setOffsetInBytes(int byteOffset, bool now);

public:
   friend SPtr<Stream> AudioSystem::generateStream(const SPtr<AudioSource> &source, UPtr<StreamDataSource> dataSource);

   Stream(Stream &&other);

   Stream& operator=(Stream &&other);

   virtual ~Stream() override = default;

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
