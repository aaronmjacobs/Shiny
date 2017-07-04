#ifndef SHINY_TEXTURE_MATERIAL_H
#define SHINY_TEXTURE_MATERIAL_H

#include "Shiny/Pointers.h"
#include "Shiny/Graphics/Material.h"

#include <string>

namespace Shiny {

class Texture;

class TextureMaterial : public Material {
public:
   TextureMaterial(const SPtr<Texture>& inTexture, const std::string& inUniformName);

   virtual void apply(ShaderProgram& program, RenderData& renderData) override;

   void setTexture(const SPtr<Texture>& newTexture);

private:
   SPtr<Texture> texture;
   std::string uniformName;
};

} // namespace Shiny

#endif
