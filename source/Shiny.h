#ifndef SHINY_H
#define SHINY_H

namespace Shiny {

/**
 * Possible results of Shiny functions
 */
enum class Result {
   kOK,
   kWorkingDirectory,
   kGlfwInit,
   kGladLoad,
   kWindowParams,
   kCreateWindow
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

/**
 * Loads OpenGL - must be called after an OpenGL context is created
 */
Result loadGL();

} // namespace Shiny

#endif
