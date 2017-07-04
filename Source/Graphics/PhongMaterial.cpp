#include "Shiny/Graphics/PhongMaterial.h"
#include "Shiny/Graphics/ShaderProgram.h"

namespace Shiny {

void PhongMaterial::apply(ShaderProgram& shaderProgram, RenderData& renderData) {
   shaderProgram.setUniformValue("uMaterial.ambient", ambient);
   shaderProgram.setUniformValue("uMaterial.diffuse", diffuse);
   shaderProgram.setUniformValue("uMaterial.specular", specular);
   shaderProgram.setUniformValue("uMaterial.emissive", emissive);
   shaderProgram.setUniformValue("uMaterial.shininess", shininess);
}

} // namespace Shiny
