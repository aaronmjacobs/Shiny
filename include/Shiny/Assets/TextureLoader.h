#ifndef SHINY_TEXTURE_LOADER_H
#define SHINY_TEXTURE_LOADER_H

#include "Shiny/Defines.h"
#include "Shiny/GLIncludes.h"
#include "Shiny/Pointers.h"

#include <string>
#include <unordered_map>

namespace Shiny {

class Texture;

typedef std::unordered_map<std::string, SPtr<Texture>> TextureMap;
typedef std::unordered_map<std::string, SPtr<Texture>> CubemapMap;

class SHINYAPI TextureLoader {
protected:
   TextureMap textureMap;
   CubemapMap cubemapMap;

public:
   TextureLoader();

   virtual ~TextureLoader();

   /**
    * Loads the texture with the given filename (and options), using a cached version if possible
    */
   SPtr<Texture> loadTexture(const std::string &fileName, GLenum wrap = GL_CLAMP_TO_BORDER,
                             GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR, GLenum magFilter = GL_LINEAR);

   /**
    * Loads the cubemap with the given path (and options), using a cached version if possible
    */
   SPtr<Texture> loadCubemap(const std::string &path, const std::string &extension, GLenum wrap = GL_CLAMP_TO_EDGE,
                             GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR, GLenum magFilter = GL_LINEAR);
};

} // namespace Shiny

#endif
