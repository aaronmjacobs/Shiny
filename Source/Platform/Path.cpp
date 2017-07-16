#include "Shiny/Shiny.h"
#include "Shiny/ShinyAssert.h"
#include "Shiny/Platform/OSUtils.h"
#include "Shiny/Platform/Path.h"

#include <algorithm>

namespace Shiny {

namespace {

bool isAbsolutePathStr(const std::string& pathStr) {
#ifdef _WIN32
   return pathStr.size() >= 3 && pathStr[1] == ':' && (pathStr[2] == '/' || pathStr[2] == '\\');
#else
   return !pathStr.empty() && pathStr[0] == '/';
#endif
}

bool getDirectoryStr(const std::string& pathStr, std::string& directoryStr) {
   size_t pos = pathStr.rfind('/');
   if (pos == std::string::npos) {
      return false;
   }

#ifdef _WIN32
   bool isRoot = pos == 2;
#else
   bool isRoot = pos == 0;
#endif

   if (isRoot) {
      directoryStr = pathStr;
   } else {
      directoryStr = pathStr.substr(0, pos - 1);
   }
   return true;
}

std::string cleanPathStr(const std::string& pathStr, bool relativeToExecutable = false) {
   if (pathStr.empty()) {
      return pathStr;
   }

   std::string cleanedPathStr = pathStr;

   // Convert all back slashes to forward slashes
   std::replace(cleanedPathStr.begin(), cleanedPathStr.end(), '\\', '/');

   // Eliminate trailing slashes
   while (cleanedPathStr.size() > 1 && cleanedPathStr[cleanedPathStr.size() - 1] == '/') {
      cleanedPathStr.pop_back();
   }

   // Eliminate duplicate slashes
   std::size_t duplicateSlashPos;
   while ((duplicateSlashPos = cleanedPathStr.rfind("//")) != std::string::npos) {
      cleanedPathStr.erase(duplicateSlashPos + 1);
   }

   // Make the path string absolute
   if (!isAbsolutePathStr(cleanedPathStr)) {
      std::string directoryStr;

      if (relativeToExecutable) {
         std::string executablePathStr;
         if (OSUtils::getExecutablePathString(executablePathStr)) {
            getDirectoryStr(executablePathStr, directoryStr);
         }
      } else {
         directoryStr = Path::getDataPath().toString();
      }

      ASSERT(!directoryStr.empty());
      cleanedPathStr = directoryStr + '/' + cleanedPathStr;
   }

   // Resolve all ..
   std::size_t dotsPos;
   while ((dotsPos = cleanedPathStr.find("..")) != std::string::npos) {
      std::size_t previousSlashPos = cleanedPathStr.rfind('/', dotsPos - 2);
      if (previousSlashPos == std::string::npos) {
         break;
      }

      cleanedPathStr.erase(previousSlashPos, (dotsPos - previousSlashPos) + 2);
   }

   return cleanedPathStr;
}

} // namespace

// static
Path Path::dataPath;

// static
void Path::init(const std::string& dataPathStr) {
   ASSERT(dataPath.toString().empty());

   dataPath = Path(cleanPathStr(dataPathStr, true));
}

// static
const Path& Path::getDataPath() {
   return dataPath;
}

Path::Path(const std::string& pathStr)
   : pathString(cleanPathStr(pathStr)) {
}

Path Path::getDirectory() const {
   size_t pos = pathString.rfind('/');
   ASSERT(pos != std::string::npos);

#ifdef _WIN32
   bool isRoot = pos == 2;
#else
   bool isRoot = pos == 0;
#endif

   if (isRoot) {
      return *this;
   }

   return Path(pathString.substr(0, pos));
}

Path& Path::operator/=(const std::string& otherPathStr) {
   return operator+=('/' + otherPathStr);
}

Path Path::operator/(const std::string& otherPathStr) const {
   Path newPath(*this);
   newPath /= otherPathStr;
   return newPath;
}

Path& Path::operator+=(const std::string& otherPathStr) {
   pathString = cleanPathStr(pathString + otherPathStr);
   return *this;
}

Path Path::operator+(const std::string& otherPathStr) const {
   Path newPath(*this);
   newPath += otherPathStr;
   return newPath;
}

} // namespace Shiny
