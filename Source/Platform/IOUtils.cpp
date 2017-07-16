#include "Shiny/ShinyAssert.h"
#include "Shiny/Platform/IOUtils.h"
#include "Shiny/Platform/OSUtils.h"

#include <fstream>

namespace Shiny {

namespace IOUtils {

bool canRead(const Path& path) {
   return !!std::ifstream(path.toString());
}

bool readTextFile(const Path& path, std::string& data) {
   std::ifstream in(path.toString());
   if (!in) {
      return false;
   }

   data = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
   return true;
}

std::vector<uint8_t> readBinaryFile(const Path& path) {
   std::ifstream in(path.toString(), std::ifstream::binary);
   if (!in) {
      return std::vector<uint8_t>();
   }

   std::streampos start = in.tellg();
   in.seekg(0, std::ios_base::end);
   std::streamoff size = in.tellg() - start;
   ASSERT(size >= 0, "Invalid file size");
   in.seekg(0, std::ios_base::beg);

   std::vector<uint8_t> data(static_cast<size_t>(size));
   in.read(reinterpret_cast<char*>(data.data()), size);

   return data;
}

bool writeTextFile(const Path& path, const std::string& data) {
   if (!ensurePathToFileExists(path)) {
      return false;
   }

   std::ofstream out(path.toString());
   if (!out) {
      return false;
   }

   out << data;
   return true;
}

bool writeBinaryFile(const Path& path, const std::vector<uint8_t>& data) {
   if (!ensurePathToFileExists(path)) {
      return false;
   }

   std::ofstream out(path.toString(), std::ofstream::binary);
   if (!out) {
      return false;
   }

   out.write(reinterpret_cast<const char*>(data.data()), data.size());
   return true;
}

bool appDataPath(const std::string& appName, const std::string& fileName, Path& path) {
   Path appDataDirectory;
   if (!OSUtils::getAppDataPath(appName, appDataDirectory)) {
      return false;
   }

   path = appDataDirectory / fileName;
   return true;
}

bool ensurePathToFileExists(const Path& path) {
   Path directory = path.getDirectory();

   if (OSUtils::directoryExists(directory)) {
      return true;
   }

   return OSUtils::createDirectory(directory);
}

} // namespace IOUtils

} // namespace Shiny
