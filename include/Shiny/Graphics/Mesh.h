#ifndef SHINY_MESH_H
#define SHINY_MESH_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class Mesh {
public:
   static const unsigned int kDefaultDimensionality = 3;
   static const GLenum kDefaultUsage = GL_STATIC_DRAW;

protected:
   GLuint vbo { 0 };
   GLuint nbo { 0 };
   GLuint tbo { 0 };
   GLuint ibo { 0 };
   GLuint vao { 0 };

   unsigned int numIndices { 0 };

   void release();

   void move(Mesh &&other);

public:
   Mesh();

   Mesh(const float *vertices, unsigned int numVertices, const float *normals, unsigned int numNormals,
        const float *texCoords, unsigned int numTexCoords, const unsigned int *indices, unsigned int numIndices,
        unsigned int dimensionality = kDefaultDimensionality, GLenum usage = kDefaultUsage);

   Mesh(Mesh &&other);

   Mesh& operator=(Mesh &&other);

   ~Mesh();

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
