#include "Constants.h"

#include <cstdio>

int main(int argc, char *argv[]) {
   printf("%s %d.%d.%d (%d)\n", PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, VERSION_BUILD);

   return 0;
}
