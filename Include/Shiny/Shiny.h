#ifndef SHINY_H
#define SHINY_H

#include <string>

namespace Shiny {

/**
 * Possible results of Shiny functions
 */
enum class Result {
   kOK,
   kWorkingDirectory,
   kGlfwInit
};

/**
 * Returns a string description of an error result
 */
const char* errorString(Result result);

/**
 * Initializes Shiny
 */
Result init(int argc, char* argv[]);

/**
 * Terminates Shiny
 */
void terminate();

/**
 * Returns the path to the data folder
 */
const std::string& dataPath();

} // namespace Shiny

#endif
