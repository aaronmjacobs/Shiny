#ifndef SHINY_FONT_H
#define SHINY_FONT_H

#include "Shiny/Pointers.h"

#include <string>

namespace Shiny {

class Font {
protected:
   UPtr<unsigned char[]> fontData;

public:
   Font(UPtr<unsigned char[]> data);

   unsigned char* data();
};

} // namespace Shiny

#endif
