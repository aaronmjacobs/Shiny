#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Texture.h"

#include "Shiny/Text/Font.h"
#include "Shiny/Text/FontAtlas.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT ASSERT
#include <stb/stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_assert ASSERT
#include <stb/stb_truetype.h>

#include <array>

namespace Shiny {

struct FontPackData {
   std::vector<stbtt_packedchar> charData;
};

namespace {

const int kMaxGlyphs = ('~' - ' ') + 1;
const int kBaseTextureSize = 64;
const int kMaxTextureSize = 2048;

struct CategoryData {
   const Glyph::Category id;
   const char firstGlyph;
   const char lastGlyph;
   const int numGlyphs;

   CategoryData(Glyph::Category id, char firstGlyph, char lastGlyph)
      : id(id), firstGlyph(firstGlyph), lastGlyph(lastGlyph), numGlyphs((lastGlyph - firstGlyph) + 1){}
};

const std::array<const CategoryData, 8> kCategoryData = {{
   CategoryData(Glyph::kUpperCase, 'A', 'Z'),
   CategoryData(Glyph::kLowerCase, 'a', 'z'),
   CategoryData(Glyph::kNumbers, '0', '9'),
   CategoryData(Glyph::kSymbols, '!', '/'),
   CategoryData(Glyph::kSymbols, ':', '@'),
   CategoryData(Glyph::kSymbols, '[', '`'),
   CategoryData(Glyph::kSymbols, '{', '~'),
   CategoryData(Glyph::kSpace, ' ', ' ')
}};

int copyGlyphs(std::array<int, kMaxGlyphs> &glyphs, int start, int end, int count) {
   ASSERT(count + (end - start) < kMaxGlyphs, "Trying to copy too many glyphs");

   for (int i = start; i <= end; ++i, ++count) {
      glyphs[count] = i;
   }

   return count;
}

int glyphsForCategory(Glyph::Category mask, std::array<int, kMaxGlyphs> &glyphs) {
   int count = 0;

   for (const CategoryData &category : kCategoryData) {
      if (mask & category.id) {
         count = copyGlyphs(glyphs, category.firstGlyph, category.lastGlyph, count);
      }
   }

   return count;
}

Glyph::Category categorize(char c) {
   for (const CategoryData &category : kCategoryData) {
      if (c >= category.firstGlyph && c <= category.lastGlyph) {
         return category.id;
      }
   }

   return Glyph::kNone;
}

int charIndex(char c, Glyph::Category mask) {
   Glyph::Category charMaskId = categorize(c);
   if (!(charMaskId & mask)) {
      // If the character is not in the generated atlas, give an invalid index
      return -1;
   }

   int index = 0;

   for (const CategoryData &category : kCategoryData) {
      // If the category is not in the generated atlas, skip over it
      if (!(category.id & mask)) {
         continue;
      }

      if (charMaskId == category.id) {
         // If the character is in the current category, increase the index by its offset
         index += (c - category.firstGlyph);
      } else if(charMaskId > category.id) {
         // If the character is in a category later than the current one, increase the index by the number of glyphs
         // in the category
         index += category.numGlyphs;
      }
   }

   return index;
}

bool pack(const SPtr<Font> &font, stbtt_pack_context *packContext, stbtt_pack_range *packRange,
          UPtr<unsigned char[]> *textureData, int *width, int *height) {
   *width = kBaseTextureSize;
   *height = kBaseTextureSize;

   while (true) {
      *textureData = UPtr<unsigned char[]>(new unsigned char[*width * *height]);

      int res = stbtt_PackBegin(packContext, textureData->get(), *width, *height, 0, 1, nullptr);
      if (!res) {
         return false;
      }

      res = stbtt_PackFontRanges(packContext, font->data(), 0, packRange, 1);
      if (res) {
         stbtt_PackEnd(packContext);
         return true;
      }

      if (*width == kMaxTextureSize && *height == kMaxTextureSize) {
         return false;
      }

      if (*width > *height) {
         *height *= 2;
      } else {
         *width *= 2;
      }
   }
}

} // namespace

FontAtlas::FontAtlas(const SPtr<Font> &font, float fontSize, Glyph::Category glyphCategory)
   : font(font), fontSize(fontSize), glyphCategory(glyphCategory), packData(new FontPackData),
     textureWidth(0), textureHeight(0), generated(false) {
   ASSERT(font, "Trying to create FontAtlas with null font");
   ASSERT(fontSize > 0.0f, "Trying to create FontAtlas with invalid font size: %f", fontSize);
   ASSERT((glyphCategory & ~Glyph::kAll) == 0, "Trying to create FontAtlas with invalid glyph category: 0x%X",
          glyphCategory);
}

FontAtlas::~FontAtlas() {
}

bool FontAtlas::generate() {
   generated = false;

   std::array<int, kMaxGlyphs> glyphs;
   int numGlyphs = glyphsForCategory(glyphCategory, glyphs);
   packData->charData.resize(numGlyphs);

   stbtt_pack_range packRange;
   packRange.font_size = fontSize;
   packRange.first_unicode_codepoint_in_range = 0;
   packRange.array_of_unicode_codepoints = glyphs.data();
   packRange.num_chars = numGlyphs;
   packRange.chardata_for_range = packData->charData.data();

   UPtr<unsigned char[]> textureData;
   stbtt_pack_context packContext;

   bool packed = pack(font, &packContext, &packRange, &textureData, &textureWidth, &textureHeight);
   if (!packed) {
      return false;
   }

   texture = std::make_shared<Texture>(GL_TEXTURE_2D);
   texture->bind();

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureData.get());

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   texture->unbind();

   generated = true;
   return true;
}

std::vector<GlyphQuad> FontAtlas::process(const std::string &text) const {
   ASSERT(generated, "Trying to process text with font atlas that is not generated");

   std::vector<GlyphQuad> quads;
   float x = 0.0f, y = 0.0f;

   for (char c : text) {
      int index = charIndex(c, glyphCategory);
      if (index < 0) {
         continue;
      }

      stbtt_aligned_quad q;
      stbtt_GetPackedQuad(packData->charData.data(), textureWidth, textureHeight, index, &x, &y, &q, 0);
      quads.push_back({ q.x0, q.y0, q.s0, q.t0, q.x1, q.y1, q.s1, q.t1 });
   }

   return quads;
}

} // namespace Shiny
