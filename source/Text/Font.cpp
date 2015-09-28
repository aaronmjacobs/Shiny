#include "Shiny/ShinyAssert.h"

#include "Shiny/Text/Font.h"

namespace Shiny {

Font::Font(UPtr<unsigned char[]> data)
   : fontData(std::move(data)) {
   ASSERT(fontData, "Trying to create font with null data");
}

Font::~Font() {
}

unsigned char* Font::data() {
   return fontData.get();
}

} // namespace Shiny
