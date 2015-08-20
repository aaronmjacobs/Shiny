#ifndef SHINY_IOUTILS_H
#define SHINY_IOUTILS_H

#include "Constants.h"
#include "Pointers.h"

#include <string>

namespace Shiny {

/**
 * IO utility / helper functions
 */
namespace IOUtils {

/**
 * Gets the absolute path of a resource, given a relative path
 */
inline std::string dataPath(const std::string &fileName) {
   return DATA_DIR "/" + fileName;
}

/**
 * Gets the absolute path of a resource stored in the app data folder, given a relative path, returning true on success
 */
bool appDataPath(const std::string &fileName, std::string &path);

/**
 * Determines if the file with the given name can be read
 */
bool canRead(const std::string &fileName);

/**
 * Determines if the file with the given (relative to the data directory) name can be read
 */
bool canReadData(const std::string &fileName);

/**
 * Reads the entire contents of the text file with the given name
 */
bool readFromFile(const std::string &fileName, std::string &data);

/**
 * Reads the entire contents of the text file with the given name, relative to the data directory,
 * returning true on success
 */
bool readFromDataFile(const std::string &fileName, std::string &data);

/**
 * Reads the entire contents of the binary file with the given name
 */
UPtr<unsigned char[]> readFromBinaryFile(const std::string &fileName);

/**
 * Reads the entire contents of the binary file with the given name, relative to the data directory
 */
UPtr<unsigned char[]> readFromBinaryDataFile(const std::string &fileName);

/**
 * Writes the contents of the given text to the file with the given name, returning true on success
 */
bool writeToFile(const std::string &fileName, const std::string &data);

/**
 * Writes the contents of the given text to the file with the given name, relative to the data directory,
 * returning true on success
 */
bool writeToDataFile(const std::string &fileName, const std::string &data);

} // namespace IOUtils

} // namespace Shiny

#endif
