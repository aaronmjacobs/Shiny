#ifndef SHINY_IOUTILS_H
#define SHINY_IOUTILS_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include <ios>
#include <string>

namespace Shiny {

/**
 * IO utility / helper functions
 */
namespace IOUtils {

/**
 * Determines if the file with the given name can be read
 */
SHINYAPI bool canRead(const std::string &fileName);

/**
 * Reads the entire contents of the text file with the given name
 */
SHINYAPI bool readTextFile(const std::string &fileName, std::string &data);

/**
 * Reads the entire contents of the binary file with the given name, storing the number of bytes in numBytes if set
 */
SHINYAPI UPtr<unsigned char[]> readBinaryFile(const std::string &fileName, size_t *numBytes = nullptr);

/**
 * Writes the contents of the given text to the file with the given name, returning true on success
 */
SHINYAPI bool writeTextFile(const std::string &fileName, const std::string &data);

/**
 * Writes the contents of the given array to the file with the given name, returning true on success
 */
SHINYAPI bool writeBinaryFile(const std::string &fileName, unsigned char *data, size_t numBytes);

/**
 * Gets the absolute path of a resource stored in the app data folder given a relative path and application name,
 * returning true on success
 */
SHINYAPI bool appDataPath(const std::string &appName, const std::string &fileName, std::string &path);

} // namespace IOUtils

} // namespace Shiny

#endif
