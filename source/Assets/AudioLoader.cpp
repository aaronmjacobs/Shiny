#include "Shiny/ShinyAssert.h"

#include "Shiny/Assets/AudioLoader.h"

#include "Shiny/Audio/AudioBuffer.h"
#include "Shiny/Audio/AudioSource.h"

#include "Shiny/Platform/IOUtils.h"

#include <stb/stb_vorbis.c>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

namespace Shiny {

namespace {

enum class AudioFileType {
   kUnknown,
   kWav,
   kOggVorbis
};

struct WavChunkInfo {
   std::array<char, 5> id;
   int32_t size;
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

template<typename Integer>
Integer readWavInt(const unsigned char *source, long *offset) {
   Integer value;

   memcpy(&value, source + *offset, sizeof(value));
   *offset += sizeof(value);

   return value;
}

template<size_t size>
std::array<char, size + 1> readWavString(const unsigned char *source, long *offset) {
   std::array<char, size + 1> string;
   string[size] = '\0';

   memcpy(string.data(), source + *offset, size);
   *offset += size;

   return string;
}

WavChunkInfo readChunkHeader(const unsigned char *data, long *offset) {
   return { readWavString<4>(data, offset), readWavInt<typeof(WavChunkInfo::size)>(data, offset) };
}

WavChunkInfo findChunk(const unsigned char *data, long *offset, long numBytes, const char *name) {
   ASSERT(strlen(name) == 4, "Invalid WAV chunk name: %s", name);

   WavChunkInfo chunkInfo = {};

   do {
      *offset += chunkInfo.size;
      chunkInfo = readChunkHeader(data, offset);
   } while (strcmp(chunkInfo.id.data(), name) != 0 && *offset < numBytes);

   return chunkInfo;
}

SPtr<AudioBuffer> loadWavBuffer(const AudioSystem &audioSystem, const unsigned char *data, long numBytes) {
   enum WaveFormat : uint16_t {
      kPCM = 0x0001,
      kIEEEFloat = 0x0003,
      kALaw = 0x0006,
      kMuLaw = 0x0007,
      kExtensible = 0xFFFE
   };

   long offset = 0;

   // RIFF chunk
   WavChunkInfo riffChunkInfo = findChunk(data, &offset, numBytes, "RIFF");
   if (offset >= numBytes || offset + riffChunkInfo.size > numBytes) {
      // Chunk not found, or not enough data remaining to read chunk
      return nullptr;
   }

   // WAVEID
   std::array<char, 5> waveID = readWavString<4>(data, &offset);
   if (strcmp(waveID.data(), "WAVE") != 0) {
      // Bad header data
      return nullptr;
   }

   // Format chunk
   WavChunkInfo formatChunkInfo = findChunk(data, &offset, numBytes, "fmt ");
   if (offset >= numBytes || offset + formatChunkInfo.size > numBytes) {
      // Chunk not found, or not enough data remaining to read chunk
      return nullptr;
   }

   uint16_t formatTag = readWavInt<uint16_t>(data, &offset);
   if (formatTag != kPCM) {
      // Only PCM currently supported
      return nullptr;
   }

   int16_t numChannels = readWavInt<int16_t>(data, &offset);
   int32_t sampleRate = readWavInt<int32_t>(data, &offset);
   offset += 6; // 6 bytes we don't care about (avg bytes per second and block align)
   int16_t bitsPerSample = readWavInt<int16_t>(data, &offset);

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
      return nullptr;
   }

   // Data chunk
   WavChunkInfo dataChunkInfo = findChunk(data, &offset, numBytes, "data");
   if (offset >= numBytes || offset + dataChunkInfo.size > numBytes) {
      // Chunk not found, or not enough data remaining to read chunk
      return nullptr;
   }

   SPtr<AudioBuffer> buffer(audioSystem.generateBuffer());
   buffer->setData(format, data + offset, dataChunkInfo.size, sampleRate);

   return buffer;
}

UPtr<short[]> loadVorbisFromMemory(const unsigned char *data, long numBytes, VorbisInfo *info) {
   ASSERT(info, "Passed null info pointer");
   ASSERT(numBytes >= 0, "Invalid number of bytes: %ld", numBytes);
   ASSERT(data || numBytes == 0, "Invalid data pointer");

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

SPtr<AudioSource> AudioLoader::loadSound(const AudioSystem &audioSystem, const std::string &fileName) {
   SPtr<AudioBuffer> buffer(loadBuffer(audioSystem, fileName));
   if (!buffer) {
      // Failed to load, use empty buffer
      buffer = audioSystem.generateBuffer();
   }

   SPtr<AudioSource> source(audioSystem.generateSource());
   source->setCurrentBuffer(buffer);

   return source;
}

} // namespace Shiny
