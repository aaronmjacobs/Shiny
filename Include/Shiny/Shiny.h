#ifndef SHINY_H
#define SHINY_H

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
Result init();

/**
 * Terminates Shiny
 */
void terminate();

} // namespace Shiny

#endif
