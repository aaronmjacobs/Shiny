#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Assets/AudioLoader.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioSource.h"
#include "Shiny/Audio/Sound.h"
#include "Shiny/Audio/Stream.h"

#include "Shiny/Platform/IOUtils.h"

#include <stb/stb_vorbis.c>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>

namespace Shiny {

namespace {

// Metadata

enum class AudioFileType {
   kUnknown,
   kWav,
   kOggVorbis
};

struct WavInfo {
   int sampleRate;
   int dataChunkSize;
   AudioBuffer::Format format;
};

struct VorbisInfo {
   int numChannels;
   int numSamples;
   int sampleRate;
   AudioBuffer::Format format;
};

std::string lowerCase(const std::string &str) {
   std::string lower(str);

   std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

   return lower;
}

bool endsWith(const std::string &str, const std::string &suffix) {
   return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

AudioFileType determineFileType(const std::string &fileName) {
   static const std::string kWavSuffix = "wav";
   static const std::string kOggSuffix = "ogg";

   std::string fileNameLower(lowerCase(fileName));

   if (endsWith(fileNameLower, kWavSuffix)) {
      return AudioFileType::kWav;
   }

   if (endsWith(fileNameLower, kOggSuffix)) {
      return AudioFileType::kOggVorbis;
   }

   return AudioFileType::kUnknown;
}

// WavReader

class WavReader {
private:
   struct ChunkInfo {
      std::array<char, 5> id;
      int32_t size;
   };

   bool findChunk(const char *name, ChunkInfo *chunkInfo);

public:
   virtual size_t read(void *dest, size_t numBytes) = 0;

   virtual bool skip(size_t numBytes) = 0;

   virtual void reset() = 0;

   template<typename T>
   bool readStruct(T *dest) {
      return read(dest, sizeof(*dest)) == sizeof(*dest);
   }

   bool readString(std::array<char, 5> *dest);

   bool readHeader(WavInfo *wavInfo);
};

bool WavReader::findChunk(const char *name, ChunkInfo *chunkInfo) {
   ASSERT(strlen(name) == 4, "Invalid WAV chunk name: %s", name);

   chunkInfo->size = 0;

   bool chunkRead = true;
   do {
      chunkRead = chunkRead && skip(chunkInfo->size);
      chunkRead = chunkRead && readString(&chunkInfo->id);
      chunkRead = chunkRead && readStruct(&chunkInfo->size);
   } while (chunkRead && strcmp(chunkInfo->id.data(), name) != 0);

   return chunkRead;
}

bool WavReader::readString(std::array<char, 5> *dest) {
   dest->at(dest->size() - 1) = '\0';
   return read(dest->data(), dest->size() - 1) == dest->size() - 1;
}

bool WavReader::readHeader(WavInfo *wavInfo) {
   enum WaveFormat : uint16_t {
      kPCM = 0x0001,
      kIEEEFloat = 0x0003,
      kALaw = 0x0006,
      kMuLaw = 0x0007,
      kExtensible = 0xFFFE
   };

   reset();

   // RIFF chunk
   ChunkInfo riffChunkInfo;
   if (!findChunk("RIFF", &riffChunkInfo)) {
      return false;
   }

   // WAVEID
   std::array<char, 5> waveStr;
   if (!readString(&waveStr) || strcmp(waveStr.data(), "WAVE") != 0) {
      return false;
   }

   // Format chunk
   ChunkInfo formatChunkInfo;
   if (!findChunk("fmt ", &formatChunkInfo)) {
      return false;
   }

   uint16_t formatTag;
   if (!readStruct(&formatTag) || formatTag != kPCM) {
      // Only PCM currently supported
      return false;
   }

   int16_t numChannels;
   if (!readStruct(&numChannels)) {
      return false;
   }
   int32_t sampleRate;
   if (!readStruct(&sampleRate)) {
      return false;
   }
   skip(6); // 6 bytes we don't care about (avg bytes per second and block align)
   int16_t bitsPerSample;
   if (!readStruct(&bitsPerSample)) {
      return false;
   }

   AudioBuffer::Format format;
   if (numChannels == 1 && bitsPerSample == 8) {
      format = AudioBuffer::Format::kMono8;
   } else if (numChannels == 1 && bitsPerSample == 16) {
      format = AudioBuffer::Format::kMono16;
   } else if (numChannels == 2 && bitsPerSample == 8) {
      format = AudioBuffer::Format::kStereo8;
   } else if (numChannels == 2 && bitsPerSample == 16) {
      format = AudioBuffer::Format::kStereo16;
   } else {
      // Unsupported number of channels / bits per sample
      return false;
   }

   // Data chunk
   ChunkInfo dataChunkInfo;
   if (!findChunk("data", &dataChunkInfo)) {
      return false;
   }

   wavInfo->sampleRate = sampleRate;
   wavInfo->dataChunkSize = dataChunkInfo.size;
   wavInfo->format = format;

   return true;
}

// WavMemReader

class WavMemReader : public WavReader {
private:
   const unsigned char *memory;
   const long memSize;
   long offset;

public:
   WavMemReader(const unsigned char *memory, const long memSize);

   virtual size_t read(void *dest, size_t numBytes) override;

   virtual bool skip(size_t numBytes) override;

   virtual void reset() override;

   const unsigned char* getCurrentPos() const {
      return memory + offset;
   }
};

WavMemReader::WavMemReader(const unsigned char *memory, const long memSize)
   : memory(memory), memSize(memSize), offset(0) {
}

size_t WavMemReader::read(void *dest, size_t numBytes) {
   size_t bytesRead = offset + numBytes > memSize ? memSize - offset : numBytes;

   memcpy(dest, memory + offset, bytesRead);
   offset += bytesRead;

   return bytesRead;
}

bool WavMemReader::skip(size_t numBytes) {
   size_t bytesSkipped = offset + numBytes > memSize ? memSize - offset : numBytes;

   offset += bytesSkipped;

   return bytesSkipped == numBytes;
}

void WavMemReader::reset() {
   offset = 0;
}

// WavFileReader

class WavFileReader : public WavReader {
private:
   mutable std::ifstream file;
   std::streampos start;

public:
   WavFileReader(const std::string &fileName);

   virtual size_t read(void *dest, size_t numBytes) override;

   virtual bool skip(size_t numBytes) override;

   virtual void reset() override;

   std::streamoff getCurrentPos() const {
      return file.tellg() - start;
   }
};

WavFileReader::WavFileReader(const std::string &fileName)
   : file(fileName, std::ifstream::binary), start(file.tellg()) {
}

size_t WavFileReader::read(void *dest, size_t numBytes) {
   file.read(reinterpret_cast<char*>(dest), numBytes);
   std::streamsize bytesRead = file.gcount();

   return bytesRead;
}

bool WavFileReader::skip(size_t numBytes) {
   std::streampos currentPos = file.tellg();
   std::streampos desiredNewPos = currentPos + std::streamoff(numBytes);

   file.seekg(desiredNewPos);
   std::streampos actualNewPos = file.tellg();

   // Note: if seekg goes past the end of the file, this will still be true - failure will not be detected until the
   // next read operation
   return desiredNewPos == actualNewPos;
}

void WavFileReader::reset() {
   file.clear();
   file.seekg(0, std::ios_base::beg);
}

class NullDataSource : public StreamDataSource {
public:
   virtual bool fill(AudioBuffer *buffer) override { return false; }

   virtual bool seekTo(int offset) override { return offset == 0; }

   virtual int getOffset() const override { return 0; }

   virtual int getSize() const override { return 1; }

   virtual int getNumChannels() const override { return 1; }

   virtual int getBitsPerSample() const override { return 8; }

   virtual int getFrequency() const override { return 1; }
};

class WavFileDataSource : public StreamDataSource {
private:
   WavFileReader reader;
   WavInfo info;
   std::streamoff dataOffset;
   bool initialized;

public:
   WavFileDataSource(const std::string &fileName);

   virtual ~WavFileDataSource();

   bool initialize();

   virtual bool fill(AudioBuffer *buffer) override;

   virtual bool seekTo(int offset) override;

   virtual int getOffset() const override;

   virtual int getSize() const override;

   virtual int getNumChannels() const override;

   virtual int getBitsPerSample() const override;

   virtual int getFrequency() const override;
};

WavFileDataSource::WavFileDataSource(const std::string &fileName)
   : reader(fileName), info({0}), dataOffset(0), initialized(false) {
}

WavFileDataSource::~WavFileDataSource() {
}

bool WavFileDataSource::initialize() {
   ASSERT(!initialized, "Trying to initialize an already-initialized WavFileDataSource");

   reader.reset();
   if (!reader.readHeader(&info)) {
      return false;
   }
   dataOffset = reader.getCurrentPos();

   initialized = true;
   return true;
}

bool WavFileDataSource::fill(AudioBuffer *buffer) {
   ASSERT(initialized, "Trying to fill buffer with an uninitialized WavFileDataSource");

   static constexpr unsigned int kDefaultBufferSize = 16384;

   std::array<unsigned char, kDefaultBufferSize> data;
   size_t bytesRead = reader.read(data.data(), data.size());

   buffer->setData(info.format, data.data(), bytesRead, info.sampleRate);

   return bytesRead > 0;
}

bool WavFileDataSource::seekTo(int offset) {
   ASSERT(initialized, "Trying to seek to offset with an uninitialized WavFileDataSource");

   reader.reset();
   return reader.skip(dataOffset + offset);
}

int WavFileDataSource::getOffset() const {
   ASSERT(initialized, "Trying to get offset of an uninitialized WavFileDataSource");

   int readerPos = static_cast<int>(reader.getCurrentPos());
   ASSERT(readerPos >= -1, "Invalid reader pos: %d", readerPos);

   // readerPos will be -1 when end-of-file has been reached
   return readerPos == -1 ? getSize() : readerPos - static_cast<int>(dataOffset);
}

int WavFileDataSource::getSize() const {
   ASSERT(initialized, "Trying to get size of an uninitialized WavFileDataSource");

   return static_cast<int>(info.dataChunkSize);
}

int WavFileDataSource::getNumChannels() const {
   ASSERT(initialized, "Trying to get number of channels of an uninitialized WavFileDataSource");

   return info.format == AudioBuffer::Format::kMono8 || info.format == AudioBuffer::Format::kMono16 ? 1 : 2;
}

int WavFileDataSource::getBitsPerSample() const {
   ASSERT(initialized, "Trying to get bits per sample of an uninitialized WavFileDataSource");

   return info.format == AudioBuffer::Format::kMono8 || info.format == AudioBuffer::Format::kStereo8 ? 8 : 16;
}

int WavFileDataSource::getFrequency() const {
   ASSERT(initialized, "Trying to get frequency of an uninitialized WavFileDataSource");

   return info.sampleRate;
}

// Buffer loading

SPtr<AudioBuffer> loadWavBuffer(const AudioSystem &audioSystem, const unsigned char *data, long numBytes) {
   WavMemReader reader(data, numBytes);
   WavInfo info;
   if (!reader.readHeader(&info)) {
      return nullptr;
   }

   SPtr<AudioBuffer> buffer(audioSystem.generateBuffer());
   buffer->setData(info.format, reader.getCurrentPos(), info.dataChunkSize, info.sampleRate);

   return buffer;
}

UPtr<short[]> loadVorbisFromMemory(const unsigned char *data, long numBytes, VorbisInfo *info) {
   short *audioData = nullptr;
   info->numSamples = stb_vorbis_decode_memory(data, (int)numBytes, &info->numChannels, &info->sampleRate, &audioData);
   info->format = info->numChannels == 1 ? AudioBuffer::Format::kMono16 : AudioBuffer::Format::kStereo16;

   return UPtr<short[]>(audioData);
}

SPtr<AudioBuffer> loadVorbisBuffer(const AudioSystem &audioSystem, const unsigned char *data, long numBytes) {
   VorbisInfo info;
   UPtr<short[]> audioData(loadVorbisFromMemory(data, numBytes, &info));
   if (info.numSamples < 0) {
      return nullptr;
   }

   SPtr<AudioBuffer> buffer(audioSystem.generateBuffer());
   buffer->setData(info.format, audioData.get(), info.numSamples * info.numChannels * sizeof(short), info.sampleRate);

   return buffer;
}

SPtr<AudioBuffer> loadBuffer(const AudioSystem &audioSystem, const std::string &fileName) {
   AudioFileType fileType = determineFileType(fileName);
   if (fileType == AudioFileType::kUnknown) {
      return nullptr;
   }

   long numBytes = 0;
   UPtr<unsigned char[]> fileData(IOUtils::readBinaryFile(fileName, &numBytes));
   if (!fileData) {
      return nullptr;
   }

   SPtr<AudioBuffer> buffer;
   switch (fileType) {
      case AudioFileType::kWav:
         buffer = loadWavBuffer(audioSystem, fileData.get(), numBytes);
         break;
      case AudioFileType::kOggVorbis:
         buffer = loadVorbisBuffer(audioSystem, fileData.get(), numBytes);
         break;
      default:
         ASSERT(false, "Invalid audio type, shouldn't be able to get here!");
   }

   return buffer;
}

} // namespace

AudioLoader::AudioLoader() {
}

AudioLoader::~AudioLoader() {
}

SPtr<Sound> AudioLoader::loadSound(AudioSystem &audioSystem, const std::string &fileName) {
   SPtr<AudioBuffer> buffer(loadBuffer(audioSystem, fileName));
   if (!buffer) {
      // Failed to load, use empty buffer
      buffer = audioSystem.generateBuffer();
      LOG_WARNING("Unable to load sound from file \"" << fileName << "\", reverting to default (nothing)");
   }

   SPtr<AudioSource> source(audioSystem.generateSource());
   source->setCurrentBuffer(buffer);

   return audioSystem.generateSound(source);
}

SPtr<Stream> AudioLoader::loadStream(AudioSystem &audioSystem, const std::string &fileName) {
   UPtr<StreamDataSource> dataSource;
   UPtr<WavFileDataSource> wavDataSource(new WavFileDataSource(fileName));
   if (wavDataSource->initialize()) {
      dataSource = std::move(wavDataSource);
   } else {
      LOG_WARNING("Unable to load stream from file \"" << fileName << "\", reverting to default (nothing)");
      dataSource = UPtr<NullDataSource>(new NullDataSource);
   }

   const int kNumBuffers = 2;
   std::vector<SPtr<AudioBuffer>> buffers(kNumBuffers);
   for (SPtr<AudioBuffer> &buffer : buffers) {
      buffer = audioSystem.generateBuffer();
   }

   SPtr<AudioSource> source(audioSystem.generateSource());
   source->queueBuffers(buffers);

   return audioSystem.generateStream(source, std::move(dataSource));
}

} // namespace Shiny
