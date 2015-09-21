#ifndef SHINY_MESH_H
#define SHINY_MESH_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class SHINYAPI Mesh {
protected:
   GLuint vbo;
   GLuint nbo;
   GLuint ibo;
   GLuint tbo;
   GLuint vao;

   UPtr<float[]> vertices;
   UPtr<unsigned int[]> indices;

   unsigned int numVertices;
   unsigned int numIndices;
   unsigned int dimensionality;

public:
   Mesh(UPtr<float[]> vertices, unsigned int numVertices, UPtr<float[]> normals, unsigned int numNormals,
        UPtr<unsigned int[]> indices, unsigned int numIndices, UPtr<float[]> texCoords, unsigned int numTexCoords,
        unsigned int dimensionality = 3, GLenum usage = GL_STATIC_DRAW);

   Mesh(Mesh &&other);

   virtual ~Mesh();

   GLuint getVBO() const {
     return vbo;
   }

   GLuint getNBO() const {
     return nbo;
   }

   GLuint getIBO() const {
     return ibo;
   }

   GLuint getTBO() const {
      return tbo;
   }

   GLuint getVAO() const {
      return vao;
   }

   float* getVertices() const {
      return vertices.get();
   }

   unsigned int getNumVertices() const {
      return numVertices;
   }

   unsigned int* getIndices() const {
      return indices.get();
   }

   unsigned int getNumIndices() const {
     return numIndices;
   }

   unsigned int getDimensionality() const {
      return dimensionality;
   }
};

} // namespace Shiny

#endif
