#ifndef SHINY_TEXTURE_MATERIAL_H
#define SHINY_TEXTURE_MATERIAL_H

#include "Shiny/Pointers.h"

#include "Shiny/Graphics/Material.h"

#include <string>

namespace Shiny {

class Texture;

class TextureMaterial : public Material {
protected:
   SPtr<Texture> texture;
   std::string uniformName;

public:
   TextureMaterial(const SPtr<Texture> &texture, const std::string &uniformName);

   virtual ~TextureMaterial();

   virtual void apply(const SPtr<ShaderProgram> &program, RenderData &renderData);
};

} // namespace Shiny

#endif
