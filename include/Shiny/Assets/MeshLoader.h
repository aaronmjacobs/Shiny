#ifndef SHINY_MESH_LOADER_H
#define SHINY_MESH_LOADER_H

#include "Shiny/Defines.h"
#include "Shiny/Pointers.h"

#include <string>
#include <unordered_map>

namespace Shiny {

class Mesh;

typedef std::unordered_map<std::string, SPtr<Mesh>> MeshMap;

enum class SHINYAPI MeshShape {
   Cube, XYPlane
};

class SHINYAPI MeshLoader {
protected:
   MeshMap meshMap;

public:
   MeshLoader();

   virtual ~MeshLoader();

   /**
    * Loads the mesh with the given file name, using a cached version if possible
    */
   SPtr<Mesh> loadMesh(const std::string &fileName);

   /**
    * Gets a mesh with the given shape
    */
   SPtr<Mesh> getMeshForShape(MeshShape shape);
};

} // namespace Shiny

#endif
