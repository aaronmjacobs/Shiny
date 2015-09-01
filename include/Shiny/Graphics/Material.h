#ifndef SHINY_MATERIAL_H
#define SHINY_MATERIAL_H

#include "Shiny/Pointers.h"

namespace Shiny {

class RenderData;
class ShaderProgram;

class Material {
public:
   Material() {}

   virtual ~Material() {}

   virtual void apply(const SPtr<ShaderProgram> &program, RenderData &renderData) = 0;
};

} // namespace Shiny

#endif
