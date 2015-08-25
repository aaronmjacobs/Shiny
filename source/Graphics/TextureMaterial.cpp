#include "TextureMaterial.h"

#include "RenderData.h"
#include "ShaderProgram.h"
#include "ShinyAssert.h"
#include "Texture.h"

namespace Shiny {

TextureMaterial::TextureMaterial(const SPtr<Texture> &texture, const std::string &uniformName)
   : texture(texture), uniformName(uniformName) {
   ASSERT(texture, "Trying to create TextureMaterial from null texture");
}

TextureMaterial::~TextureMaterial() {
}

void TextureMaterial::apply(const SPtr<ShaderProgram> &program, RenderData &renderData) {
   ASSERT(program, "Trying to apply TextureMaterial to null ShaderProgram");

   GLint textureUnit = renderData.aquireTextureUnit();

   glActiveTexture(GL_TEXTURE0 + textureUnit);
   texture->bind();

   program->setUniformValue(uniformName, textureUnit);
}

} // namespace Shiny
