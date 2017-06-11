#ifndef SHINY_TEXT_RENDERER_H
#define SHINY_TEXT_RENDERER_H

#include "Shiny/Pointers.h"

#include "Shiny/Graphics/Model.h"

namespace Shiny {

class FontAtlas;
class ShaderProgram;
class Texture;
class TextureMaterial;

class TextRenderer {
protected:
   SPtr<FontAtlas> atlas;
   SPtr<TextureMaterial> textureMaterial;
   Model model;

public:
   TextRenderer(const SPtr<FontAtlas> &atlas, const SPtr<ShaderProgram> &program);

   SPtr<Texture> renderToTexture(const char *text, int *textureWidth = nullptr, int *textureHeight = nullptr);

   void setShaderProgram(const SPtr<ShaderProgram> &program);
};

} // namespace Shiny

#endif
