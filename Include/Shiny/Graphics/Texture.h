#ifndef SHINY_TEXTURE_H
#define SHINY_TEXTURE_H

#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Graphics/TextureInfo.h"

#include <glm/glm.hpp>

namespace Shiny {

class Texture {
public:
   Texture(const Tex::Specification& textureSpecification);
   Texture(const Texture& other) = delete;
   Texture(Texture&& other);

   ~Texture();

   Texture& operator=(const Texture& other) = delete;
   Texture& operator=(Texture&& other);

   GLuint getId() const {
      return id;
   }

   const Tex::Specification& getSpecification() const {
      return specification;
   }

   void updateSpecification(const Tex::Specification& textureSpecification);

   void setParam(Tex::FloatParam param, GLfloat value);
   void setParam(Tex::IntParam param, GLint value);
   void setParam(Tex::FloatArrayParam param, glm::vec4 value);
   void setParam(Tex::IntArrayParam param, glm::ivec4 value);
   void setParam(Tex::InternalIntArrayParam param, glm::ivec4 value);
   void setParam(Tex::InternalUintArrayParam param, glm::uvec4 value);

   void generateMipMaps();

   void bind() {
      Context::current()->bindTexture(specification.target, id);
   }
   void unbind() {
      Context::current()->bindTexture(specification.target, 0);
   }

private:
   GLuint id;
   Tex::Specification specification;

   void release();
   void move(Texture &&other);
};

} // namespace Shiny

#endif
