#ifndef SHINY_TEXTURE_MATERIAL_H
#define SHINY_TEXTURE_MATERIAL_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include "Shiny/Graphics/Material.h"

#include <string>

namespace Shiny {

class Texture;

class SHINYAPI TextureMaterial : public Material {
protected:
   SPtr<Texture> texture;
   std::string uniformName;

public:
   TextureMaterial(const SPtr<Texture> &texture, const std::string &uniformName);

   virtual ~TextureMaterial();

   virtual void apply(const SPtr<ShaderProgram> &program, RenderData &renderData) override;

   void setTexture(const SPtr<Texture> &texture);
};

} // namespace Shiny

#endif
