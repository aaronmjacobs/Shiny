#include "Model.h"

#include "GLIncludes.h"
#include "Mesh.h"
#include "ShaderProgram.h"

namespace Shiny {

Model::Model() {
}

Model::Model(const SPtr<Mesh> &mesh, const SPtr<ShaderProgram> &program)
   : mesh(mesh), program(program) {
}

Model::Model(Model &&other)
   : mesh(other.mesh), program(other.program) {
   other.mesh = nullptr;
   other.program = nullptr;
}

Model::~Model() {
}

void Model::draw() {
   if (mesh && program) {
      glBindVertexArray(mesh->getVAO());

      program->commit();
      glDrawElements(GL_TRIANGLES, mesh->getNumIndices(), GL_UNSIGNED_INT, 0);
   }
}

void Model::setMesh(const SPtr<Mesh> &mesh) {
   this->mesh = mesh;
}

void Model::setShaderProgram(const SPtr<ShaderProgram> &program) {
   this->program = program;
}

} // namespace Shiny
