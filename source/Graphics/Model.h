#ifndef SHINY_MODEL_H
#define SHINY_MODEL_H

#include "GLIncludes.h"
#include "Mesh.h"
#include "Pointers.h"
#include "ShaderProgram.h"

namespace Shiny {

class Model {
private:
   SPtr<Mesh> mesh;
   SPtr<ShaderProgram> program;

public:
   Model(const SPtr<Mesh> &mesh, const SPtr<ShaderProgram> &program);

   Model(Model &&other);

   virtual ~Model();

   void draw();

   ShaderProgram& getProgram() {
      return *program;
   }
};

} // namespace Shiny

#endif
