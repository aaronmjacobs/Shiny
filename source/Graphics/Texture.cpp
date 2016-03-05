#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Texture.h"

namespace Shiny {

Texture::Texture(GLenum target)
   : target(target) {
   ASSERT(target == GL_TEXTURE_1D ||
          target == GL_TEXTURE_2D ||
          target == GL_TEXTURE_3D ||
          target == GL_TEXTURE_1D_ARRAY ||
          target == GL_TEXTURE_2D_ARRAY ||
          target == GL_TEXTURE_RECTANGLE ||
          target == GL_TEXTURE_CUBE_MAP ||
          target == GL_TEXTURE_BUFFER ||
          target == GL_TEXTURE_2D_MULTISAMPLE ||
          target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
          "Invalid texture target: %u", target);

   glGenTextures(1, &textureID);
}

Texture::Texture(Texture &&other) {
   move(std::move(other));
}

Texture& Texture::operator=(Texture &&other) {
   release();
   move(std::move(other));
   return *this;
}

Texture::~Texture() {
   release();
}

void Texture::release() {
   glDeleteTextures(1, &textureID);
}

void Texture::move(Texture &&other) {
   textureID = other.textureID;
   target = other.target;

   other.textureID = 0;
}

void Texture::bind() {
   glBindTexture(target, textureID);
}

void Texture::unbind() {
   glBindTexture(target, 0);
}

} // namespace Shiny
