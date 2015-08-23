#include "Mesh.h"

#include "ShaderProgram.h"
#include "ShinyAssert.h"

namespace Shiny {

Mesh::Mesh(UPtr<float[]> vertices, unsigned int numVertices, UPtr<float[]> normals, unsigned int numNormals,
           UPtr<unsigned int[]> indices, unsigned int numIndices, UPtr<float[]> texCoords, unsigned int numTexCoords,
           unsigned int dimensionality, GLenum usage)
   : vbo(0), nbo(0), ibo(0), tbo(0), vao(0), numVertices(numVertices), numIndices(numIndices), dimensionality(dimensionality) {
   ASSERT(numVertices == 0 || vertices, "numVertices > 0, but no vertices provided");
   ASSERT(numNormals == 0 || normals, "numNormals > 0, but no normals provided");
   ASSERT(numIndices == 0 || indices, "numIndices > 0, but no indices provided");
   ASSERT(numTexCoords == 0 || texCoords, "numTexCoords > 0, but no texCoords provided");
   ASSERT(dimensionality >= 1 && dimensionality <= 4, "dimensionality must be between 1 and 4 (inclusive)");
   ASSERT(usage == GL_STATIC_DRAW || usage == GL_DYNAMIC_DRAW, "Invalid usage: %u", usage);

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   if (numVertices > 0) {
      glGenBuffers(1, &vbo);

      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dimensionality * numVertices, vertices.get(), usage);

      glEnableVertexAttribArray(ShaderAttributes::POSITION);
      glVertexAttribPointer(ShaderAttributes::POSITION, dimensionality, GL_FLOAT, GL_FALSE, 0, 0);
   }

   if (numNormals > 0) {
      glGenBuffers(1, &nbo);

      glBindBuffer(GL_ARRAY_BUFFER, nbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dimensionality * numNormals, normals.get(), usage);

      glEnableVertexAttribArray(ShaderAttributes::NORMAL);
      glVertexAttribPointer(ShaderAttributes::NORMAL, dimensionality, GL_FLOAT, GL_FALSE, 0, 0);
   }

   if (numIndices > 0) {
      glGenBuffers(1, &ibo);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indices.get(), usage);
   }

   if (numTexCoords > 0) {
      glGenBuffers(1, &tbo);

      glBindBuffer(GL_ARRAY_BUFFER, tbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numTexCoords, texCoords.get(), usage);

      glEnableVertexAttribArray(ShaderAttributes::TEX_COORD);
      glVertexAttribPointer(ShaderAttributes::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
   }

   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   this->vertices = std::move(vertices);
   this->indices = std::move(indices);
}

Mesh::Mesh(Mesh &&other)
   : vbo(other.vbo), nbo(other.nbo), ibo(other.ibo), tbo(other.tbo), vao(other.vao), vertices(std::move(other.vertices)),
     indices(std::move(other.indices)), numVertices(other.numVertices), numIndices(other.numIndices),
     dimensionality(other.dimensionality) {
   other.vbo = other.nbo = other.ibo = other.tbo = other.vao = other.numVertices = other.numIndices = other.dimensionality = 0;
}

Mesh::~Mesh() {
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &nbo);
   glDeleteBuffers(1, &ibo);
   glDeleteBuffers(1, &tbo);
   glDeleteVertexArrays(1, &vao);
}

} // namespace Shiny
