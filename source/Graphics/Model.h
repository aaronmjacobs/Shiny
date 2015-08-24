#ifndef SHINY_MODEL_H
#define SHINY_MODEL_H

#include "Pointers.h"

namespace Shiny {

class Mesh;
class ShaderProgram;

class Model {
private:
   SPtr<Mesh> mesh;
   SPtr<ShaderProgram> program;

public:
   Model();

   Model(const SPtr<Mesh> &mesh, const SPtr<ShaderProgram> &program);

   Model(Model &&other);

   virtual ~Model();

   void draw();

   const SPtr<Mesh>& getMesh() const {
      return mesh;
   }

   const SPtr<ShaderProgram>& getProgram() const {
      return program;
   }

   void setMesh(const SPtr<Mesh> &mesh);

   void setShaderProgram(const SPtr<ShaderProgram> &program);
};

} // namespace Shiny

#endif
