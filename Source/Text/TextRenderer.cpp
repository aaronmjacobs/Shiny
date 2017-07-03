#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Framebuffer.h"
#include "Shiny/Graphics/Mesh.h"
#include "Shiny/Graphics/RenderData.h"
#include "Shiny/Graphics/ShaderProgram.h"
#include "Shiny/Graphics/TextureMaterial.h"

#include "Shiny/Text/Font.h"
#include "Shiny/Text/FontAtlas.h"
#include "Shiny/Text/TextRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace Shiny {

namespace {

void render(const GlyphQuad &quad, Model &model, float yOffset) {
   std::array<float, 12> vertices = {{ quad.x0, quad.y1 + yOffset, 0.0f,
                                       quad.x1, quad.y1 + yOffset, 0.0f,
                                       quad.x0, quad.y0 + yOffset, 0.0f,
                                       quad.x1, quad.y0 + yOffset, 0.0f }};

   std::array<float, 8> texCoords = {{ quad.s0, quad.t1,
                                       quad.s1, quad.t1,
                                       quad.s0, quad.t0,
                                       quad.s1, quad.t0 }};

   std::array<unsigned int, 6> indices = {{ 0, 1, 2,
                                            2, 1, 3 }};

   model.getMesh()->setVertices(vertices.data(), static_cast<unsigned int>(vertices.size()) / 3);
   model.getMesh()->setTexCoords(texCoords.data(), static_cast<unsigned int>(texCoords.size()) / 2);
   model.getMesh()->setIndices(indices.data(), static_cast<unsigned int>(indices.size()));

   RenderData renderData;
   model.draw(renderData);
}

} // namespace

TextRenderer::TextRenderer(const SPtr<FontAtlas> &atlas, const SPtr<ShaderProgram> &program)
   : atlas(atlas), textureMaterial(std::make_shared<TextureMaterial>(nullptr, "uTexture")),
     model(std::make_shared<Mesh>(), program) {
   ASSERT(atlas, "Trying to create TextRenderer with null atlas");
   ASSERT(program, "Trying to create TextRenderer with null shader program");

   model.attachMaterial(textureMaterial);
}

SPtr<Texture> TextRenderer::renderToTexture(const char *text, int *textureWidth, int *textureHeight) {
   if (!atlas->isGenerated()) {
      bool generated = atlas->generate();
      if (!generated) {
         return nullptr;
      }
   }

   float width, height;
   std::vector<GlyphQuad> quads(atlas->process(text, &width, &height));
   if (textureWidth) {
      *textureWidth = static_cast<int>(glm::ceil(width));
   }
   if (textureHeight) {
      *textureHeight = static_cast<int>(glm::ceil(height));
   }

   const char *uProjMatrix = "uProjMatrix";
   if (model.getShaderProgram()->hasUniform(uProjMatrix)) {
      // stb_truetype generates the bitmap top-down, but we need it to be bottom-up,
      // so just flip the y values using the projection matrix
      model.getShaderProgram()->setUniformValue(uProjMatrix, glm::ortho<float>(0.0f, width, height, 0.0f));
   }

   UPtr<Framebuffer> framebuffer = std::make_unique<Framebuffer>(static_cast<GLsizei>(glm::ceil(width)), static_cast<GLsizei>(glm::ceil(height)), false);
   framebuffer->bind();
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT);
   glDisable(GL_DEPTH_TEST);

   textureMaterial->setTexture(atlas->getTexture());
   FontSpacing spacing = atlas->getFontSpacing();
   for (const GlyphQuad &quad : quads) {
      render(quad, model, spacing.descent);
   }

   glEnable(GL_DEPTH_TEST);
   Framebuffer::bindDefaultFramebuffer();

   return framebuffer->getColorAttachment(0);
}

void TextRenderer::setShaderProgram(const SPtr<ShaderProgram> &program) {
   ASSERT(program, "Trying to set null shader program in TextRenderer");
   model.setShaderProgram(program);
}

} // namespace Shiny
