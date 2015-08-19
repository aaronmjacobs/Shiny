#include "Model.h"

#include "ShinyAssert.h"

namespace Shiny {

Model::Model(const SPtr<Mesh> &mesh, const SPtr<ShaderProgram> &program)
   : mesh(mesh), program(program) {
   ASSERT(mesh, "null mesh passed to model");
   ASSERT(program, "null program passed to model");

   glGenVertexArrays(1, &vao);

   glBindVertexArray(vao);

   glBindBuffer(GL_ARRAY_BUFFER, mesh->getVBO());
   glEnableVertexAttribArray(ShaderAttributes::POSITION);
   glVertexAttribPointer(ShaderAttributes::POSITION, mesh->getDimensionality(), GL_FLOAT, GL_FALSE, 0, 0);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getIBO());
}

Model::Model(Model &&other)
   : vao(other.vao), mesh(other.mesh), program(other.program) {
   other.vao = 0;
   other.mesh = nullptr;
   other.program = nullptr;
}

Model::~Model() {
   glDeleteVertexArrays(1, &vao);
}

void Model::draw() {
   glBindVertexArray(vao);

   program->commit();
   glDrawElements(GL_TRIANGLES, mesh->getNumIndices(), GL_UNSIGNED_INT, 0);
}

} // namespace Shiny
