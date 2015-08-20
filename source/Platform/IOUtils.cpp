#include "IOUtils.h"

#include "OSUtils.h"
#include "ShinyAssert.h"

#include <fstream>

namespace Shiny {

namespace IOUtils {

bool appDataPath(const std::string &fileName, std::string &path) {
   std::string appDataFolder;
   if (!OSUtils::getAppDataPath(appDataFolder)) {
      return false;
   }

   path = appDataFolder + "/" + fileName;
   return true;
}

bool canRead(const std::string &fileName) {
   ASSERT(!fileName.empty(), "Trying to check empty file name");
   return !!std::ifstream(fileName);
}

bool canReadData(const std::string &fileName) {
   ASSERT(!fileName.empty(), "Trying to check empty file name");
   return canRead(dataPath(fileName));
}

bool readFromFile(const std::string &fileName, std::string &data) {
   ASSERT(!fileName.empty(), "Trying to read from empty file name");
   std::ifstream in(fileName);
   if (!in) {
      return false;
   }

   data = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
   return true;
}

bool writeToFile(const std::string &fileName, const std::string &data) {
   ASSERT(!fileName.empty(), "Trying to write to empty file name");
   std::ofstream out(fileName);
   if (!out) {
      return false;
   }

   out << data;
   return true;
}

UPtr<unsigned char[]> readFromBinaryFile(const std::string &fileName) {
   ASSERT(!fileName.empty(), "Trying to read from empty file name");
   std::ifstream in(fileName, std::ifstream::binary);
   if (!in) {
      return nullptr;
   }

   in.seekg(0, std::ios_base::end);
   std::size_t size = in.tellg();
   in.seekg(0, std::ios_base::beg);

   UPtr<unsigned char[]> data(new unsigned char[size]);
   in.read((char*)data.get(), size);

   return std::move(data);
}

UPtr<unsigned char[]> readFromBinaryDataFile(const std::string &fileName) {
   ASSERT(!fileName.empty(), "Trying to read from empty file name");
   return std::move(readFromBinaryFile(dataPath(fileName)));
}

bool readFromDataFile(const std::string &fileName, std::string &data) {
   ASSERT(!fileName.empty(), "Trying to read from empty file name");
   return readFromFile(dataPath(fileName), data);
}

bool writeToDataFile(const std::string &fileName, const std::string &data) {
   ASSERT(!fileName.empty(), "Trying to write to empty file name");
   return writeToFile(dataPath(fileName), data);
}

} // namespace IOUtils

} // namespace Shiny
