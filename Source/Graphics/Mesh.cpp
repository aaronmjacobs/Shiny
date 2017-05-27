#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/Mesh.h"
#include "Shiny/Graphics/ShaderProgram.h"

namespace Shiny {

namespace {

void prepareBuffer(GLuint *buffer, GLenum target, unsigned int numValues, unsigned int dimensionality, const void *data,
                   GLenum usage, ShaderAttributes::Attributes atrributes = ShaderAttributes::kPosition) {
   ASSERT(buffer, "Trying to prepare null buffer");
   ASSERT(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER, "Invalid buffer target: %u", target);
   ASSERT(dimensionality >= 1 && dimensionality <= 4, "dimensionality must be between 1 and 4 (inclusive): %u",
          dimensionality);
   ASSERT(numValues == 0 || data, "numValues > 0, but no data provided");
   ASSERT(usage == GL_STATIC_DRAW || usage == GL_DYNAMIC_DRAW, "Invalid usage: %u", usage);

   if (*buffer == 0 && numValues == 0) {
      // Buffer hasn't been generated and there are no values to set - no need to do anything
      return;
   }

   if (*buffer == 0) {
      glGenBuffers(1, buffer);
   }

   size_t valueSize = target == GL_ELEMENT_ARRAY_BUFFER ? sizeof(unsigned int) : sizeof(float);

   glBindBuffer(target, *buffer);
   glBufferData(target, numValues * dimensionality * valueSize, data, usage);

   if (target == GL_ARRAY_BUFFER) {
      glEnableVertexAttribArray(atrributes);
      glVertexAttribPointer(atrributes, dimensionality, GL_FLOAT, GL_FALSE, 0, 0);
   }
}

} // namespace

Mesh::Mesh()
   : vbo(0), nbo(0), tbo(0), ibo(0), vao(0), numIndices(0) {
   glGenVertexArrays(1, &vao);
}

Mesh::Mesh(const float *vertices, unsigned int numVertices, const float *normals, unsigned int numNormals,
           const float *texCoords, unsigned int numTexCoords, const unsigned int *indices, unsigned int numIndices,
           unsigned int dimensionality, GLenum usage)
   : numIndices(numIndices) {
   glGenVertexArrays(1, &vao);

   setVertices(vertices, numVertices, dimensionality, usage);
   setNormals(normals, numNormals, dimensionality, usage);
   setTexCoords(texCoords, numTexCoords, usage);
   setIndices(indices, numIndices, usage);
}

Mesh::Mesh(Mesh &&other) {
   move(std::move(other));
}

Mesh& Mesh::operator=(Mesh &&other) {
   release();
   move(std::move(other));
   return *this;
}

Mesh::~Mesh() {
   release();
}

void Mesh::release() {
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &nbo);
   glDeleteBuffers(1, &tbo);
   glDeleteBuffers(1, &ibo);
   glDeleteVertexArrays(1, &vao);
}

void Mesh::move(Mesh &&other) {
   vbo = other.vbo;
   nbo = other.nbo;
   tbo = other.tbo;
   ibo = other.ibo;
   vao = other.vao;
   numIndices = other.numIndices;

   other.vbo = 0;
   other.nbo = 0;
   other.tbo = 0;
   other.ibo = 0;
   other.vao = 0;
   other.numIndices = 0;
}

void Mesh::bindVAO() const {
   ASSERT(vao > 0, "Trying to bind invalid VAO");
   Context::current()->bindVertexArray(vao);
}

void Mesh::draw() const {
   bindVAO();
   glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void Mesh::setVertices(const float *vertices, unsigned int numVertices, unsigned int dimensionality, GLenum usage) {
   bindVAO();
   prepareBuffer(&vbo, GL_ARRAY_BUFFER, numVertices, dimensionality, vertices, usage, ShaderAttributes::kPosition);
}

void Mesh::setNormals(const float *normals, unsigned int numNormals,
                      unsigned int dimensionality, GLenum usage) {
   bindVAO();
   prepareBuffer(&nbo, GL_ARRAY_BUFFER, numNormals, dimensionality, normals, usage, ShaderAttributes::kNormal);
}

void Mesh::setTexCoords(const float *texCoords, unsigned int numTexCoords, GLenum usage) {
   bindVAO();
   prepareBuffer(&tbo, GL_ARRAY_BUFFER, numTexCoords, 2, texCoords, usage, ShaderAttributes::kTexCoord);
}

void Mesh::setIndices(const unsigned int *indices, unsigned int numIndices, GLenum usage) {
   this->numIndices = numIndices;

   bindVAO();
   prepareBuffer(&ibo, GL_ELEMENT_ARRAY_BUFFER, numIndices, 1, indices, usage);
}

} // namespace Shiny
