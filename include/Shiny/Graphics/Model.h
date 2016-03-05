#ifndef SHINY_MODEL_H
#define SHINY_MODEL_H

#include "Shiny/Pointers.h"

#include "Shiny/Graphics/RenderData.h"

#include <vector>

namespace Shiny {

class Material;
class Mesh;
class ShaderProgram;

typedef std::vector<SPtr<Material>> MaterialVector;

class Model {
private:
   SPtr<Mesh> mesh;
   SPtr<ShaderProgram> program;
   MaterialVector materials;

   void move(Model &&other);

public:
   Model() = default;

   Model(const SPtr<Mesh> &mesh, const SPtr<ShaderProgram> &program);

   Model(Model &&other);

   Model& operator=(Model &&other);

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
