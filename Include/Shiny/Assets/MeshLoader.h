#ifndef SHINY_MESH_LOADER_H
#define SHINY_MESH_LOADER_H

#include "Shiny/Pointers.h"

#include <string>
#include <unordered_map>

namespace Shiny {

class Mesh;

typedef std::unordered_map<std::string, SPtr<Mesh>> MeshMap;

enum class MeshShape {
   Cube, XYPlane
};

class MeshLoader {
protected:
   MeshMap meshMap;

   SPtr<Mesh> cubeMesh;
   SPtr<Mesh> xyPlaneMesh;

public:
   /**
    * Loads the mesh with the given file name, using a cached version if possible
    */
   SPtr<Mesh> loadMesh(const std::string &fileName, bool generateNormalsIfMissing = true);

   /**
    * Gets a mesh with the given shape
    */
   SPtr<Mesh> getMeshForShape(MeshShape shape);
};

} // namespace Shiny

#endif
