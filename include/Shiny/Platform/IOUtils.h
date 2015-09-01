#ifndef SHINY_IOUTILS_H
#define SHINY_IOUTILS_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include <string>

namespace Shiny {

/**
 * IO utility / helper functions
 */
namespace IOUtils {

/**
 * Gets the absolute path of a resource stored in the app data folder, given a relative path, returning true on success
 */
SHINYAPI bool appDataPath(const std::string &fileName, std::string &path);

/**
 * Determines if the file with the given name can be read
 */
SHINYAPI bool canRead(const std::string &fileName);

/**
 * Reads the entire contents of the text file with the given name
 */
SHINYAPI bool readFromFile(const std::string &fileName, std::string &data);

/**
 * Reads the entire contents of the binary file with the given name
 */
SHINYAPI UPtr<unsigned char[]> readFromBinaryFile(const std::string &fileName);

/**
 * Writes the contents of the given text to the file with the given name, returning true on success
 */
SHINYAPI bool writeToFile(const std::string &fileName, const std::string &data);

} // namespace IOUtils

} // namespace Shiny

#endif
