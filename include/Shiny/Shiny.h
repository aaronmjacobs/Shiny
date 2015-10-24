#ifndef SHINY_H
#define SHINY_H

#include "Shiny/Defines.h"

namespace Shiny {

/**
 * Possible results of Shiny functions
 */
enum class SHINYAPI Result {
   kOK,
   kWorkingDirectory,
   kGlfwInit
};

/**
 * Returns a string description of an error result
 */
SHINYAPI const char* errorString(Result result);

/**
 * Initializes Shiny
 */
SHINYAPI Result init();

/**
 * Terminates Shiny
 */
SHINYAPI void terminate();

} // namespace Shiny

#endif
