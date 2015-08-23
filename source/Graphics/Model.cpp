#include "Model.h"

#include "ShinyAssert.h"

namespace Shiny {

Model::Model(const SPtr<Mesh> &mesh, const SPtr<ShaderProgram> &program)
   : mesh(mesh), program(program) {
   ASSERT(mesh, "null mesh passed to model");
   ASSERT(program, "null program passed to model");
}

Model::Model(Model &&other)
   : mesh(other.mesh), program(other.program) {
   other.mesh = nullptr;
   other.program = nullptr;
}

Model::~Model() {
}

void Model::draw() {
   glBindVertexArray(mesh->getVAO());

   program->commit();
   glDrawElements(GL_TRIANGLES, mesh->getNumIndices(), GL_UNSIGNED_INT, 0);
}

} // namespace Shiny
