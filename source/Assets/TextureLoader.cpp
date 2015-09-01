#include "TextureLoader.h"

#include "DefaultImageSource.h"
#include "Log.h"
#include "ShinyAssert.h"
#include "Graphics/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT ASSERT
#include <stb/stb_image.h>

#include <functional>

namespace Shiny {

namespace {

typedef UPtr<unsigned char[], std::function<decltype(stbi_image_free)>> PixelPtr;

struct ImageInfo {
   int width;
   int height;
   int composition;
   GLint format;
   PixelPtr pixels;
};

PixelPtr createPixelPtr(unsigned char *pixels) {
   return PixelPtr(pixels, stbi_image_free);
}

bool determineFormat(ImageInfo &info) {
   switch (info.composition) {
      case 1:
         info.format = GL_R8;
         return true;
      case 2:
         info.format = GL_RG16;
         return true;
      case 3:
         info.format = GL_RGB;
         return true;
      case 4:
         info.format = GL_RGBA;
         return true;
      default:
         info.format = GL_RGBA;
         return false;
   }
}

ImageInfo getDefaultImageInfo() {
   ImageInfo defaultImageInfo;

   defaultImageInfo.pixels = createPixelPtr(stbi_load_from_memory(kDefaultImageSource, kDefaultImageSourceSize,
                                                                  &defaultImageInfo.width, &defaultImageInfo.height,
                                                                  &defaultImageInfo.composition, 0));

   if (!defaultImageInfo.pixels || !determineFormat(defaultImageInfo)) {
      LOG_FATAL("Unable to load default texture");
   }

   return defaultImageInfo;
}

ImageInfo loadImage(const std::string &fileName) {
   ImageInfo info;

   info.pixels = createPixelPtr(stbi_load(fileName.c_str(), &info.width, &info.height, &info.composition, 0));

   if (!info.pixels || !determineFormat(info)) {
      LOG_WARNING("Unable to load image from file: " << fileName);
      info = getDefaultImageInfo();
   }

   return info;
}

void setParameters(GLenum target, GLenum wrap, GLenum minFilter, GLenum magFilter) {
   ASSERT(target == GL_TEXTURE_2D ||
          target == GL_TEXTURE_CUBE_MAP,
          "Invalid texture target enum");
   ASSERT(wrap == GL_CLAMP_TO_EDGE ||
          wrap == GL_CLAMP_TO_BORDER ||
          wrap == GL_MIRRORED_REPEAT ||
          wrap == GL_REPEAT,
          "Invalid texture wrap enum");
   ASSERT(minFilter == GL_NEAREST ||
          minFilter == GL_LINEAR ||
          minFilter == GL_NEAREST_MIPMAP_NEAREST ||
          minFilter == GL_LINEAR_MIPMAP_NEAREST ||
          minFilter == GL_NEAREST_MIPMAP_LINEAR ||
          minFilter == GL_LINEAR_MIPMAP_LINEAR,
          "Invalid texture min filter enum");
   ASSERT(magFilter == GL_NEAREST ||
          magFilter == GL_LINEAR,
          "Invalid texture mag filter enum");

   if (minFilter == GL_NEAREST_MIPMAP_NEAREST ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_LINEAR) {
      glGenerateMipmap(target);
   }

   glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
   glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
   glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);

   glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
   glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
}

void storeTexture(GLenum target, const ImageInfo &info) {
   ASSERT(target == GL_TEXTURE_2D ||
          target == GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
          target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
          target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
          target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
          target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z ||
          target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
          "Invalid texture target enum");

   glTexImage2D(target, 0, info.format, info.width, info.height, 0, info.format, GL_UNSIGNED_BYTE, info.pixels.get());
}

} // namespace

TextureLoader::TextureLoader() {
}

TextureLoader::~TextureLoader() {
}

SPtr<Texture> TextureLoader::loadTexture(const std::string &fileName, GLenum wrap, GLenum minFilter, GLenum magFilter) {
   TextureMap::iterator location(textureMap.find(fileName));
   if (location != textureMap.end()) {
      return location->second;
   }

   // Load images bottom-to-top (since that is how OpenGL expects textures)
   stbi_set_flip_vertically_on_load(true);

   ImageInfo info = loadImage(fileName);

   SPtr<Texture> texture(std::make_shared<Texture>(GL_TEXTURE_2D));
   texture->bind();

   storeTexture(GL_TEXTURE_2D, info);

   setParameters(GL_TEXTURE_2D, wrap, minFilter, magFilter);

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

   const std::string &rightName = "/right." + extension;
   const std::string &leftName = "/left." + extension;
   const std::string &upName = "/up." + extension;
   const std::string &downName = "/down." + extension;
   const std::string &backName = "/back." + extension;
   const std::string &frontName = "/front." + extension;

   ImageInfo right = loadImage(path + rightName);
   ImageInfo left = loadImage(path + leftName);
   ImageInfo up = loadImage(path + upName);
   ImageInfo down = loadImage(path + downName);
   ImageInfo back = loadImage(path + backName);
   ImageInfo front = loadImage(path + frontName);

   SPtr<Texture> cubemap(std::make_shared<Texture>(GL_TEXTURE_CUBE_MAP));
   cubemap->bind();

   storeTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
   storeTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
   storeTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, up);
   storeTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, down);
   storeTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
   storeTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);

   setParameters(GL_TEXTURE_CUBE_MAP, wrap, minFilter, magFilter);

   cubemap->unbind();

   cubemapMap.insert({ path, cubemap });
   return cubemap;
}

} // namespace Shiny
