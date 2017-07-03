#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Assets/DefaultImageSource.h"
#include "Shiny/Assets/TextureLoader.h"

#include "Shiny/Graphics/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT ASSERT
#include <stb_image.h>

#include <functional>

namespace Shiny {

namespace {

typedef UPtr<unsigned char[], std::function<decltype(stbi_image_free)>> PixelPtr;

struct ImageInfo {
   int width;
   int height;
   int composition;
   PixelPtr pixels;
};

static const int kMinComposition = 1;
static const int kMaxComposition = 4;

PixelPtr createPixelPtr(unsigned char *pixels) {
   return PixelPtr(pixels, stbi_image_free);
}

bool infoMatches(const ImageInfo& first, const ImageInfo& second) {
   return first.width == second.width
      && first.height == second.height
      && first.composition == second.composition;
}

ImageInfo getDefaultImageInfo() {
   ImageInfo defaultInfo;

   defaultInfo.pixels = createPixelPtr(stbi_load_from_memory(kDefaultImageSource, kDefaultImageSourceSize,
                                                             &defaultInfo.width, &defaultInfo.height,
                                                             &defaultInfo.composition, 0));

   if (!defaultInfo.pixels || defaultInfo.composition < kMinComposition || defaultInfo.composition > kMaxComposition) {
      LOG_ERROR("Unable to load default image");
      defaultInfo.pixels = nullptr;
   }

   return defaultInfo;
}

ImageInfo loadImage(const std::string &fileName) {
   ImageInfo info;

   info.pixels = createPixelPtr(stbi_load(fileName.c_str(), &info.width, &info.height, &info.composition, 0));

   if (!info.pixels || info.composition < kMinComposition || info.composition > kMaxComposition) {
      LOG_WARNING("Unable to load image from file: " << fileName << ", reverting to default");
      info = getDefaultImageInfo();
   }

   return info;
}

void setParameters(Texture& texture, GLenum wrap, GLenum minFilter, GLenum magFilter) {
   if (minFilter == GL_NEAREST_MIPMAP_NEAREST ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_LINEAR) {
      texture.generateMipMaps();
   }

   texture.setParam(Tex::IntParam::kWrapS, wrap);
   texture.setParam(Tex::IntParam::kWrapT, wrap);
   texture.setParam(Tex::IntParam::kWrapR, wrap);

   texture.setParam(Tex::IntParam::kMinFilter, minFilter);
   texture.setParam(Tex::IntParam::kMagFilter, magFilter);
}

Tex::InternalFormat determineInternalFormat(int composition) {
   switch (composition) {
   case 1:
      return Tex::InternalFormat::kR8;
   case 2:
      return Tex::InternalFormat::kRG8;
   case 3:
      return Tex::InternalFormat::kRGB8;
   case 4:
      return Tex::InternalFormat::kRGBA8;
   default:
      ASSERT(false, "Invalid image composition: %d", composition);
      return Tex::InternalFormat::kRGB8;
   }
}

Tex::ProvidedDataFormat determineProvidedDataFormat(int composition) {
   switch (composition) {
   case 1:
      return Tex::ProvidedDataFormat::kRed;
   case 2:
      return Tex::ProvidedDataFormat::kRG;
   case 3:
      return Tex::ProvidedDataFormat::kRGB;
   case 4:
      return Tex::ProvidedDataFormat::kRGBA;
   default:
      ASSERT(false, "Invalid image composition: %d", composition);
      return Tex::ProvidedDataFormat::kRGB;
   }
}

SPtr<Texture> createTexture(const ImageInfo &info) {
   Tex::Specification specification = Tex::Specification::create2d();

   specification.internalFormat = determineInternalFormat(info.composition);
   specification.width = info.width;
   specification.height = info.height;
   specification.providedDataFormat = determineProvidedDataFormat(info.composition);
   specification.providedDataType = Tex::ProvidedDataType::kUnsignedByte;
   specification.providedData = info.pixels.get();

   return std::make_unique<Texture>(specification);
}

} // namespace

SPtr<Texture> TextureLoader::loadTexture(const std::string &fileName, GLenum wrap, GLenum minFilter, GLenum magFilter) {
   TextureMap::iterator location(textureMap.find(fileName));
   if (location != textureMap.end()) {
      return location->second;
   }

   // Load images bottom-to-top (since that is how OpenGL expects textures)
   stbi_set_flip_vertically_on_load(true);

   ImageInfo info = loadImage(fileName);

   SPtr<Texture> texture(createTexture(info));
   setParameters(*texture, wrap, minFilter, magFilter);
   texture->unbind();

   textureMap.insert({ fileName, texture });
   return texture;
}

SPtr<Texture> TextureLoader::loadCubemap(const std::string &path, const std::string &extension,
                                         GLenum wrap, GLenum minFilter, GLenum magFilter) {
   CubemapMap::iterator location(cubemapMap.find(path));
   if (location != cubemapMap.end()) {
      return location->second;
   }

   // Load images top-to-bottom because cubemaps are weird
   stbi_set_flip_vertically_on_load(false);

   std::string rightName = "/right." + extension;
   std::string leftName = "/left." + extension;
   std::string upName = "/up." + extension;
   std::string downName = "/down." + extension;
   std::string backName = "/back." + extension;
   std::string frontName = "/front." + extension;

   ImageInfo right = loadImage(path + rightName);
   ImageInfo left = loadImage(path + leftName);
   ImageInfo up = loadImage(path + upName);
   ImageInfo down = loadImage(path + downName);
   ImageInfo back = loadImage(path + backName);
   ImageInfo front = loadImage(path + frontName);

   if (!infoMatches(right, left) || !infoMatches(right, up) || !infoMatches(right, down) || !infoMatches(right, back) || !infoMatches(right, front)) {
      LOG_WARNING("Not all cubemap faces share the same image resolution, composition, or format, reverting to default");
      right = getDefaultImageInfo();
      left = getDefaultImageInfo();
      up = getDefaultImageInfo();
      down = getDefaultImageInfo();
      back = getDefaultImageInfo();
      front = getDefaultImageInfo();
   }

   Tex::Specification specification = Tex::Specification::createCubeMap();
   specification.internalFormat = determineInternalFormat(right.composition);
   specification.width = right.width;
   specification.height = right.height;
   specification.providedDataFormat = determineProvidedDataFormat(right.composition);
   specification.providedDataType = Tex::ProvidedDataType::kUnsignedByte;
   specification.positiveXData = right.pixels.get();
   specification.negativeXData = left.pixels.get();
   specification.positiveYData = up.pixels.get();
   specification.negativeYData = down.pixels.get();
   specification.positiveZData = back.pixels.get();
   specification.negativeZData = front.pixels.get();

   SPtr<Texture> cubemap(std::make_shared<Texture>(specification));
   setParameters(*cubemap, wrap, minFilter, magFilter);
   cubemap->unbind();

   cubemapMap.insert({ path, cubemap });
   return cubemap;
}

} // namespace Shiny
