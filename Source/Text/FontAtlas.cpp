#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Texture.h"

#include "Shiny/Text/Font.h"
#include "Shiny/Text/FontAtlas.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT ASSERT
#include <stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_assert ASSERT
#include <stb_truetype.h>

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

bool inCategoryData(char c, const CategoryData &data) {
   return c >= data.firstGlyph && c <= data.lastGlyph;
}

Glyph::Category categorize(char c) {
   for (const CategoryData &data : kCategoryData) {
      if (inCategoryData(c, data)) {
         return data.id;
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

   for (const CategoryData &data : kCategoryData) {
      // If the category is not in the generated atlas, skip over it
      if (!(data.id & mask)) {
         continue;
      }

      if (inCategoryData(c, data)) {
         // If the character is in the current category, increase the index by its offset
         index += (c - data.firstGlyph);
      } else if(charMaskId > data.id) {
         // If the character is in a category later than the current one, increase the index by the number of glyphs
         // in the category
         index += data.numGlyphs;
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
   : font(font), fontSize(fontSize), fontSpacing({}), glyphCategory(glyphCategory), packData(new FontPackData),
     textureWidth(0), textureHeight(0), generated(false) {
   ASSERT(font, "Trying to create FontAtlas with null font");
   ASSERT(fontSize > 0.0f, "Trying to create FontAtlas with invalid font size: %f", fontSize);
   ASSERT((glyphCategory & ~Glyph::kAll) == 0, "Trying to create FontAtlas with invalid glyph category: 0x%X",
          glyphCategory);
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

   stbtt_fontinfo fontInfo;
   int res = stbtt_InitFont(&fontInfo, font->data(), stbtt_GetFontOffsetForIndex(font->data(), 0));
   if (!res) {
      return false;
   }
   float scale = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);
   int ascent, descent, lineGap;
   stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
   fontSpacing.ascent = ascent * scale;
   fontSpacing.descent = descent * scale;
   fontSpacing.lineGap = lineGap * scale;

   Tex::Specification specification = Tex::Specification::create2d();
   specification.internalFormat = Tex::InternalFormat::kR8;
   specification.width = textureWidth;
   specification.height = textureHeight;
   specification.providedDataFormat = Tex::ProvidedDataFormat::kRed;
   specification.providedDataType = Tex::ProvidedDataType::kUnsignedByte;
   specification.providedData = textureData.get();

   texture = std::make_shared<Texture>(specification);

   texture->setParam(Tex::IntParam::kWrapS, GL_CLAMP_TO_BORDER);
   texture->setParam(Tex::IntParam::kWrapT, GL_CLAMP_TO_BORDER);
   texture->setParam(Tex::IntParam::kWrapR, GL_CLAMP_TO_BORDER);

   texture->setParam(Tex::IntParam::kMinFilter, GL_LINEAR);
   texture->setParam(Tex::IntParam::kMagFilter, GL_LINEAR);

   texture->unbind();

   generated = true;
   return true;
}

std::vector<GlyphQuad> FontAtlas::process(const std::string &text, float *width, float *height) const {
   ASSERT(generated, "Trying to process text with font atlas that is not generated");

   std::vector<GlyphQuad> quads;
   float x = 0.0f, y = fontSize;

   for (char c : text) {
      int index = charIndex(c, glyphCategory);
      if (index < 0) {
         continue;
      }

      stbtt_aligned_quad q;
      stbtt_GetPackedQuad(packData->charData.data(), textureWidth, textureHeight, index, &x, &y, &q, 0);
      quads.push_back({ q.x0, q.y0, q.s0, q.t0, q.x1, q.y1, q.s1, q.t1 });
   }

   if (width) {
      *width = x;
   }
   if (height) {
      *height = y;
   }

   return quads;
}

} // namespace Shiny
