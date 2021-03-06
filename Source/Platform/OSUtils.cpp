#include "Shiny/Platform/OSUtils.h"

#include <ctime>

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif // __APPLE__

#ifdef __linux__
#include <cstring>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif // __linux__

#ifdef _WIN32
#include <codecvt>
#include <cstring>
#include <locale>
#include <ShlObj.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <Windows.h>
#endif // _WIN32

namespace Shiny {

namespace OSUtils {

#ifdef __APPLE__
bool getExecutablePathString(std::string& executablePathStr) {
   uint32_t size = MAXPATHLEN;
   char rawPath[size];
   if (_NSGetExecutablePath(rawPath, &size) != 0) {
      return false;
   }

   char realPath[size];
   if (!realpath(rawPath, realPath)) {
      return false;
   }

   executablePathStr = std::string(realPath);
   return true;
}

bool getAppDataPath(const std::string& appName, Path& appDataPath) {
   FSRef ref;
   FSFindFolder(kUserDomain, kApplicationSupportFolderType, kCreateFolder, &ref);

   char path[PATH_MAX];
   FSRefMakePath(&ref, (UInt8*)&path, PATH_MAX);

   appDataPath = Path(std::string(path)) / appName;
   return true;
}

bool setWorkingDirectory(const Path& dir) {
   return chdir(dir.toString().c_str()) == 0;
}

bool createDirectory(const Path& dir) {
   return mkdir(dir.toString().c_str(), 0755) == 0;
}
#endif // __APPLE__

#ifdef __linux__
bool getExecutablePathString(std::string& executablePathStr) {
   char path[PATH_MAX + 1];

   ssize_t numBytes = readlink("/proc/self/exe", path, PATH_MAX);
   if (numBytes == -1) {
      return false;
   }
   path[numBytes] = '\0';

   executablePathStr = std::string(path);
   return true;
}

bool getAppDataPath(const std::string& appName, Path& appDataPath) {
   // First, check the HOME environment variable
   char* homePath = secure_getenv("HOME");

   // If it isn't set, grab the directory from the password entry file
   if (!homePath) {
      homePath = getpwuid(getuid())->pw_dir;
   }

   if (!homePath) {
      return false;
   }

   appDataPath = Path(std::string(homePath)) / (".config/" + appName);
   return true;
}

bool setWorkingDirectory(const Path& dir) {
   return chdir(dir.toString().c_str()) == 0;
}

bool createDirectory(const Path& dir) {
   return mkdir(dir.toString().c_str(), 0755) == 0;
}
#endif // __linux__

#ifdef _WIN32
bool getExecutablePathString(std::string& executablePathStr) {
   TCHAR buffer[MAX_PATH + 1];
   DWORD length = GetModuleFileName(NULL, buffer, MAX_PATH);
   buffer[length] = '\0';
   int error = GetLastError();

   if (length == 0 || length == MAX_PATH || error == ERROR_INSUFFICIENT_BUFFER) {
      const DWORD REALLY_BIG = 32767;
      TCHAR unreasonablyLargeBuffer[REALLY_BIG + 1];
      length = GetModuleFileName(NULL, unreasonablyLargeBuffer, REALLY_BIG);
      unreasonablyLargeBuffer[length] = '\0';
      error = GetLastError();

      if (length == 0 || length == REALLY_BIG || error == ERROR_INSUFFICIENT_BUFFER) {
         return false;
      }

      executablePathStr = std::string(unreasonablyLargeBuffer);
   } else {
      executablePathStr = std::string(buffer);
   }

   return true;
}

bool getAppDataPath(const std::string& appName, Path& appDataPath) {
   PWSTR path;
   if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path) != S_OK) {
      CoTaskMemFree(path);
      return false;
   }

   std::wstring widePathStr(path);
   CoTaskMemFree(path);

   std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
   appDataPath = Path(converter.to_bytes(widePathStr)) / appName;
   return true;
}

bool setWorkingDirectory(const Path& dir) {
   return SetCurrentDirectory(dir.toString().c_str()) != 0;
}

bool createDirectory(const Path& dir) {
   return CreateDirectory(dir.toString().c_str(), nullptr) != 0;
}
#endif // _WIN32

bool getDirectoryFromPath(const std::string& path, std::string& dir) {
   size_t pos = path.find_last_of("/\\");
   if (pos == std::string::npos) {
      return false;
   }

   dir = path.substr(0, pos);
   return true;
}

bool fixWorkingDirectory() {
   std::string executablePathStr;
   if (!getExecutablePathString(executablePathStr)) {
      return false;
   }

   return setWorkingDirectory(Path(executablePathStr).getDirectory());
}

bool directoryExists(const Path& dir) {
   struct stat info;

   if (stat(dir.toString().c_str(), &info) != 0) {
      return false;
   }

   return (info.st_mode & S_IFDIR) != 0;
}

int64_t getTime() {
   static_assert(sizeof(std::time_t) <= sizeof(int64_t), "std::time_t will not fit in an int64_t");
   return static_cast<int64_t>(std::time(nullptr));
}

} // namespace OSUtils

} // namespace Shiny
