#ifndef SHINY_IOUTILS_H
#define SHINY_IOUTILS_H

#include "Shiny/Pointers.h"
#include "Shiny/Platform/Path.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Shiny {

/**
 * IO utility / helper functions
 */
namespace IOUtils {

/**
 * Determines if the file with the given path can be read
 */
bool canRead(const Path& filePath);

/**
 * Reads the entire contents of the text file with the given path
 */
bool readTextFile(const Path& filePath, std::string& data);

/**
 * Reads the entire contents of the binary file with the given path
 */
std::vector<uint8_t> readBinaryFile(const Path& filePath);

/**
 * Writes the contents of the given text to the file with the given path, returning true on success
 */
bool writeTextFile(const Path& filePath, const std::string& data);

/**
 * Writes the contents of the given vector to the file with the given path, returning true on success
 */
bool writeBinaryFile(const Path& filePath, const std::vector<uint8_t>& data);

/**
 * Gets the absolute path of a resource stored in the app data folder given a relative path and application name,
 * returning true on success
 */
bool appDataPath(const std::string& appName, const std::string& fileName, Path& path);

/**
 * Ensures the path to the given file exists, returning true on success
 */
bool ensurePathToFileExists(const Path& path);

} // namespace IOUtils

} // namespace Shiny

#endif
