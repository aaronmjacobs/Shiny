#ifndef SHINY_MESH_LOADER_H
#define SHINY_MESH_LOADER_H

#include "Shiny/Pointers.h"
#include "Shiny/Platform/Path.h"

#include <unordered_map>

namespace Shiny {

class Mesh;

enum class MeshShape {
   Cube, XYPlane
};

class MeshLoader {
public:
   /**
   * Loads the mesh with the given file path, using a cached version if possible
   */
   SPtr<Mesh> loadMesh(const Path& filePath, bool generateNormalsIfMissing = true);

   /**
   * Gets a mesh with the given shape
   */
   SPtr<Mesh> getMeshForShape(MeshShape shape);

private:
   std::unordered_map<Path, SPtr<Mesh>> meshMap;

   SPtr<Mesh> cubeMesh;
   SPtr<Mesh> xyPlaneMesh;
};

} // namespace Shiny

#endif
