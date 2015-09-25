#ifndef SHINY_FONT_H
#define SHINY_FONT_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include <string>

namespace Shiny {

class SHINYAPI Font {
protected:
   UPtr<unsigned char[]> fontData;
   std::string fontName;

public:
   Font(UPtr<unsigned char[]> data, const std::string &name);

   virtual ~Font();

   unsigned char* data();

   const std::string& name() const;
};

} // namespace Shiny

#endif
