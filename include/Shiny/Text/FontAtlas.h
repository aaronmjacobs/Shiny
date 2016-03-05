#ifndef SHINY_FONT_ATLAS
#define SHINY_FONT_ATLAS

#include "Shiny/Pointers.h"

#include <string>
#include <vector>

namespace Shiny {

class Font;
class Texture;
struct FontPackData;

namespace Glyph {

enum Category {
   kNone = 0,
   kUpperCase = 1,
   kLowerCase = 2,
   kNumbers = 4,
   kSymbols = 8,
   kSpace = 16,
   kAll = kUpperCase | kLowerCase | kNumbers | kSymbols | kSpace
};

} // namespace Glyph

struct GlyphQuad {
   float x0, y0, s0, t0;
   float x1, y1, s1, t1;
};

struct FontSpacing {
   float ascent;
   float descent;
   float lineGap;
};

class FontAtlas {
protected:
   SPtr<Font> font;
   SPtr<Texture> texture;
   float fontSize;
   FontSpacing fontSpacing;
   Glyph::Category glyphCategory;

   UPtr<FontPackData> packData;
   int textureWidth;
   int textureHeight;
   bool generated;

public:
   FontAtlas(const SPtr<Font> &font, float fontSize, Glyph::Category glyphCategory);

   bool generate();

   std::vector<GlyphQuad> process(const std::string &text, float *width = nullptr, float *height = nullptr) const;

   const SPtr<Font>& getFont() const {
      return font;
   }

   const SPtr<Texture>& getTexture() const {
      return texture;
   }

   float getFontSize() const {
      return fontSize;
   }

   Glyph::Category getGlyphCategory() const {
      return glyphCategory;
   }

   FontSpacing getFontSpacing() const {
      return fontSpacing;
   }

   bool isGenerated() const {
      return generated;
   }
};

} // namespace Shiny

#endif
