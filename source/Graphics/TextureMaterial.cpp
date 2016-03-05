#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/RenderData.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Graphics/Texture.h"
#include "Shiny/Graphics/TextureMaterial.h"

namespace Shiny {

TextureMaterial::TextureMaterial(const SPtr<Texture> &texture, const std::string &uniformName)
   : texture(texture), uniformName(uniformName) {
}

void TextureMaterial::apply(const SPtr<ShaderProgram> &program, RenderData &renderData) {
   ASSERT(program, "Trying to apply TextureMaterial to null ShaderProgram");

   if (!texture) {
      return;
   }

   GLint textureUnit = renderData.aquireTextureUnit();

   glActiveTexture(GL_TEXTURE0 + textureUnit);
   texture->bind();

   program->setUniformValue(uniformName, textureUnit);
}

void TextureMaterial::setTexture(const SPtr<Shiny::Texture> &texture) {
   this->texture = texture;
}

} // namespace Shiny
