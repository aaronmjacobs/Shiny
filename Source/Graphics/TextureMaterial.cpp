#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/RenderData.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Graphics/Texture.h"
#include "Shiny/Graphics/TextureMaterial.h"

namespace Shiny {

TextureMaterial::TextureMaterial(const SPtr<Texture>& inTexture, const std::string& inUniformName)
   : texture(inTexture), uniformName(inUniformName) {
}

void TextureMaterial::apply(ShaderProgram& program, RenderData& renderData) {
   if (!texture) {
      return;
   }

   GLint textureUnit = renderData.aquireTextureUnit();
   glActiveTexture(GL_TEXTURE0 + textureUnit);
   texture->bind();

   program.setUniformValue(uniformName, textureUnit);
}

void TextureMaterial::setTexture(const SPtr<Shiny::Texture>& newTexture) {
   texture = newTexture;
}

} // namespace Shiny
