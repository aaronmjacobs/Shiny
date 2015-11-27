#include "Shiny/ShinyAssert.h"

#include "Shiny/Platform/IOUtils.h"
#include "Shiny/Platform/OSUtils.h"

#include <fstream>

namespace Shiny {

namespace IOUtils {

bool canRead(const std::string &fileName) {
   ASSERT(!fileName.empty(), "Trying to check empty file name");
   return !!std::ifstream(fileName);
}

bool readTextFile(const std::string &fileName, std::string &data) {
   ASSERT(!fileName.empty(), "Trying to read from empty file name");
   std::ifstream in(fileName);
   if (!in) {
      return false;
   }

   data = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
   return true;
}

UPtr<unsigned char[]> readBinaryFile(const std::string &fileName, size_t *numBytes) {
   ASSERT(!fileName.empty(), "Trying to read from empty file name");
   std::ifstream in(fileName, std::ifstream::binary);
   if (!in) {
      return nullptr;
   }

   std::streampos start = in.tellg();
   in.seekg(0, std::ios_base::end);
   std::streamoff size = in.tellg() - start;
   ASSERT(size >= 0, "Invalid file size");
   in.seekg(0, std::ios_base::beg);

   UPtr<unsigned char[]> data(new unsigned char[static_cast<size_t>(size)]);
   in.read(reinterpret_cast<char*>(data.get()), size);

   if (numBytes) {
      *numBytes = static_cast<size_t>(size);
   }

   return std::move(data);
}

bool writeTextFile(const std::string &fileName, const std::string &data) {
   ASSERT(!fileName.empty(), "Trying to write to empty file name");
   std::ofstream out(fileName);
   if (!out) {
      return false;
   }

   out << data;
   return true;
}

bool writeBinaryFile(const std::string &fileName, unsigned char *data, size_t numBytes) {
   ASSERT(!fileName.empty(), "Trying to write to empty file name");
   std::ofstream out(fileName, std::ofstream::binary);
   if (!out) {
      return false;
   }

   out.write(reinterpret_cast<char*>(data), numBytes);
   return true;
}

bool appDataPath(const std::string &appName, const std::string &fileName, std::string &path) {
   std::string appDataFolder;
   if (!OSUtils::getAppDataPath(appName, appDataFolder)) {
      return false;
   }

   path = appDataFolder + "/" + fileName;
   return true;
}

} // namespace IOUtils

} // namespace Shiny
