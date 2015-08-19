#include "Constants.h"
#include "Log.h"

int main(int argc, char *argv[]) {
   LOG_INFO(PROJECT_NAME << " " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_MICRO << " (" <<
            VERSION_BUILD << ")");

   return 0;
}
