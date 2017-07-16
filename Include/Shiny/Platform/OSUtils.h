#ifndef SHINY_OS_UTILS_H
#define SHINY_OS_UTILS_H

#include "Shiny/Platform/Path.h"

#include <string>

namespace Shiny {

namespace OSUtils {

/**
 * Gets the path (string) to the running executable, returning true on success
 */
bool getExecutablePathString(std::string& executablePathStr);

/**
 * Gets the path to the application's local data / settings / config folder, returning true on success
 */
bool getAppDataPath(const std::string& appName, Path& appDataPath);

/**
 * Sets the working directory of the application to the given directory, returning true on success
 */
bool setWorkingDirectory(const Path& dir);

/**
 * Sets the working directory of the application to the directory that the executable is in, returning true on success
 */
bool fixWorkingDirectory();

/**
 * Determines if the given directory exists
 */
bool directoryExists(const Path& dir);

/**
 * Creates the given directory, returning true on success
 */
bool createDirectory(const Path& dir);

} // namespace OSUtils

} // namespace Shiny

#endif
