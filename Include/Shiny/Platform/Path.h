#ifndef SHINY_PATH_H
#define SHINY_PATH_H

#include <cctype>
#include <string>

namespace Shiny {

class Path {
public:
   static void init(const std::string& dataPathStr);
   static const Path& getDataPath();

   Path() {
   }
   Path(const std::string& pathStr);
   Path(const char* pathCStr)
      : Path(std::string(pathCStr)) {
   }
   Path(const Path& directory, const std::string& pathStr)
      : Path(directory.toString() + '/' + pathStr) {
   }
   Path(const Path& directory, const char* pathCStr)
      : Path(directory, std::string(pathCStr)) {
   }

   Path(const Path& other)
      : pathString(other.pathString) {
   }
   Path(Path&& other)
      : pathString(std::move(other.pathString)) {
   }

   Path& operator=(const Path& other) {
      pathString = other.pathString;
      return *this;
   }
   Path& operator=(Path&& other) {
      pathString = std::move(other.pathString);
      return *this;
   }

   bool isValid() const {
      return !pathString.empty();
   }

   const std::string& toString() const {
      return pathString;
   }

   Path getDirectory() const;

   Path& operator/=(const std::string& otherPathStr);
   Path& operator/=(const char* otherPathCStr) {
      return operator/=(std::string(otherPathCStr));
   }

   Path operator/(const std::string& otherPathStr) const;
   Path operator/(const char* otherPathCStr) const {
      return operator/(std::string(otherPathCStr));
   }

   Path& operator+=(const std::string& otherPathStr);
   Path& operator+=(const char* otherPathCStr) {
      return operator+=(std::string(otherPathCStr));
   }

   Path operator+(const std::string& otherPathStr) const;
   Path operator+(const char* otherPathCStr) const {
      return operator+(std::string(otherPathCStr));
   }

private:
   static Path dataPath;
   std::string pathString;
};

inline bool operator==(const Path& first, const Path& second) {
#ifdef _WIN32
   return std::equal(first.toString().begin(), first.toString().end(), second.toString().begin(), [](unsigned char first, unsigned char second) {
      return std::tolower(first) == std::tolower(second);
   });
#else
   return first.toString() == second.toString();
#endif
}

inline bool operator<(const Path& first, const Path& second) {
   return first.toString() < second.toString();
}

inline std::ostream& operator<<(std::ostream& out, const Path& path) {
   return out << path.toString();
}

} // namespace Shiny

namespace std {

template<>
struct hash<Shiny::Path> {
   std::size_t operator()(const Shiny::Path& path) const {
      return hash<std::string>()(path.toString());
   }
};

} // namespace std

#endif
