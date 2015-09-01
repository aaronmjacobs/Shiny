#ifndef SHINY_MODEL_H
#define SHINY_MODEL_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include "Shiny/Graphics/RenderData.h"

#include <vector>

namespace Shiny {

class Material;
class Mesh;
class ShaderProgram;

typedef std::vector<SPtr<Material>> MaterialVector;

class SHINYAPI Model {
private:
   SPtr<Mesh> mesh;
   SPtr<ShaderProgram> program;
   MaterialVector materials;

public:
   Model();

   Model(const SPtr<Mesh> &mesh, const SPtr<ShaderProgram> &program);

   Model(Model &&other);

   virtual ~Model();

   void draw(RenderData renderData);

   const SPtr<Mesh>& getMesh() const {
      return mesh;
   }

   const SPtr<ShaderProgram>& getProgram() const {
      return program;
   }

   void setMesh(const SPtr<Mesh> &mesh);

   void setShaderProgram(const SPtr<ShaderProgram> &program);

   void attachMaterial(const SPtr<Material> &material);

   void removeMaterial(const SPtr<Material> &material);
};

} // namespace Shiny

#endif
