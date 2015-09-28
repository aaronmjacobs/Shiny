#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Material.h"
#include "Shiny/Graphics/Mesh.h"
#include "Shiny/Graphics/Model.h"
#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Graphics/ShaderProgram.h"

#include <algorithm>

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

void Model::draw(RenderData renderData) {
   if (mesh && program) {
      mesh->bindVAO();

      for (const SPtr<Material> &material : materials) {
         material->apply(program, renderData);
      }

      program->commit();
      mesh->draw();
   }
}

void Model::setMesh(const SPtr<Mesh> &mesh) {
   this->mesh = mesh;
}

void Model::setShaderProgram(const SPtr<ShaderProgram> &program) {
   this->program = program;
}

void Model::attachMaterial(const SPtr<Material> &material) {
   materials.push_back(material);
}

void Model::removeMaterial(const SPtr<Material> &material) {
   MaterialVector::iterator loc(std::find(materials.begin(), materials.end(), material));
   ASSERT(loc != materials.end(), "Material does not exist in vector");
   materials.erase(loc);
}

} // namespace Shiny
