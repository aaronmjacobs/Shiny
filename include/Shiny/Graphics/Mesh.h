#ifndef SHINY_MESH_H
#define SHINY_MESH_H

#include "Shiny/Defines.h"

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class SHINYAPI Mesh {
public:
   static const unsigned int kDefaultDimensionality = 3;
   static const GLenum kDefaultUsage = GL_STATIC_DRAW;

protected:
   GLuint vbo;
   GLuint nbo;
   GLuint tbo;
   GLuint ibo;
   GLuint vao;

   unsigned int numIndices;

public:
   Mesh();

   Mesh(const float *vertices, unsigned int numVertices, const float *normals, unsigned int numNormals,
        const float *texCoords, unsigned int numTexCoords, const unsigned int *indices, unsigned int numIndices,
        unsigned int dimensionality = kDefaultDimensionality, GLenum usage = kDefaultUsage);

   Mesh(Mesh &&other);

   virtual ~Mesh();

   void bindVAO() const;

   void draw() const;

   void setVertices(const float *vertices, unsigned int numVertices,
                    unsigned int dimensionality = kDefaultDimensionality, GLenum usage = kDefaultUsage);

   void setNormals(const float *normals, unsigned int numNormals,
                   unsigned int dimensionality = kDefaultDimensionality, GLenum usage = kDefaultUsage);

   void setTexCoords(const float *texCoords, unsigned int numTexCoords, GLenum usage = kDefaultUsage);

   void setIndices(const unsigned int *indices, unsigned int numIndices, GLenum usage = kDefaultUsage);
};

} // namespace Shiny

#endif
