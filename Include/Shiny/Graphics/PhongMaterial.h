#ifndef SHINY_PHONG_MATERIAL_H
#define SHINY_PHONG_MATERIAL_H

#include "Shiny/Graphics/Material.h"

#include <glm/glm.hpp>

namespace Shiny {

class PhongMaterial : public Material {
public:
   PhongMaterial(const glm::vec3& inAmbient, const glm::vec3& inDiffuse, const glm::vec3& inSpecular,
                 const glm::vec3& inEmissive, float inShininess)
      : ambient(inAmbient), diffuse(inDiffuse), specular(inSpecular), emissive(inEmissive), shininess(inShininess) {
   }

   virtual void apply(ShaderProgram& shaderProgram, RenderData& renderData) override;

private:
   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;
   glm::vec3 emissive;
   float shininess;
};

} // namespace Shiny

#endif
