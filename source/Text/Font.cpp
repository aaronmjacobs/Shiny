#include "Shiny/ShinyAssert.h"

#include "Shiny/Text/Font.h"

namespace Shiny {

Font::Font(UPtr<unsigned char[]> data, const std::string &name)
   : fontData(std::move(data)), fontName(name) {
   ASSERT(fontData, "Trying to create font with null data");
}

Font::~Font() {
}

unsigned char* Font::data() {
   return fontData.get();
}

const std::string& Font::name() const {
   return fontName;
}

} // namespace Shiny
