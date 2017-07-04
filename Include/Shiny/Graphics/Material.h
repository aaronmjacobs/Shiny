#ifndef SHINY_MATERIAL_H
#define SHINY_MATERIAL_H

#include "Shiny/Pointers.h"

namespace Shiny {

class RenderData;
class ShaderProgram;

class Material {
public:
   virtual ~Material() = default;

   virtual void apply(ShaderProgram& program, RenderData& renderData) = 0;
};

} // namespace Shiny

#endif
