#ifndef SHINY_FONT_H
#define SHINY_FONT_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include <string>

namespace Shiny {

class SHINYAPI Font {
protected:
   UPtr<unsigned char[]> fontData;

public:
   Font(UPtr<unsigned char[]> data);

   virtual ~Font();

   unsigned char* data();
};

} // namespace Shiny

#endif
