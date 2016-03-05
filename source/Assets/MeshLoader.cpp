#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"

#include "Shiny/Assets/MeshLoader.h"

#include "Shiny/Graphics/Mesh.h"

#include "Shiny/Platform/IOUtils.h"

#include <tinyobj/tiny_obj_loader.h>

#include <cstring>
#include <map>
#include <vector>

namespace tinyobj {

class NullMaterialReader : public MaterialReader {
public:
   NullMaterialReader() {}
   virtual ~NullMaterialReader() {}
   virtual std::string operator()(const std::string &matId,
                                  std::vector<material_t> &materials,
                                  std::map<std::string, int> &matMap) {
      return std::string("");
   }
};

} // namespace tinyobj

namespace Shiny {

namespace {

// TODO Bake in as arrays of each data type

const char* kCubeMeshSource = "v -0.500000 -0.500000 0.500000\nv 0.500000 -0.500000 0.500000\nv -0.500000 0.500000 0.500000\nv 0.500000 0.500000 0.500000\nv -0.500000 0.500000 -0.500000\nv 0.500000 0.500000 -0.500000\nv -0.500000 -0.500000 -0.500000\nv 0.500000 -0.500000 -0.500000\n\nvt 0.000000 0.000000\nvt 1.000000 0.000000\nvt 0.000000 1.000000\nvt 1.000000 1.000000\n\nvn 0.000000 0.000000 1.000000\nvn 0.000000 1.000000 0.000000\nvn 0.000000 0.000000 -1.000000\nvn 0.000000 -1.000000 0.000000\nvn 1.000000 0.000000 0.000000\nvn -1.000000 0.000000 0.000000\n\ns 1\nf 1/1/1 2/2/1 3/3/1\nf 3/3/1 2/2/1 4/4/1\ns 2\nf 3/1/2 4/2/2 5/3/2\nf 5/3/2 4/2/2 6/4/2\ns 3\nf 5/4/3 6/3/3 7/2/3\nf 7/2/3 6/3/3 8/1/3\ns 4\nf 7/1/4 8/2/4 1/3/4\nf 1/3/4 8/2/4 2/4/4\ns 5\nf 2/1/5 8/2/5 4/3/5\nf 4/3/5 8/2/5 6/4/5\ns 6\nf 7/1/6 1/2/6 5/3/6\nf 5/3/6 1/2/6 3/4/6\n";

const char* kXyPlaneMeshSource = "v -1.000000 -1.000000 -0.000000\nv 1.000000 -1.000000 -0.000000\nv -1.000000 1.000000 0.000000\nv 1.000000 1.000000 0.000000\nvt 1.000000 0.000000\nvt 1.000000 1.000000\nvt 0.000000 1.000000\nvt 0.000000 0.000000\nvn 0.000000 -0.000000 1.000000\ns off\nf 2/1/1 4/2/1 3/3/1\nf 1/4/1 2/1/1 3/3/1\n";

SPtr<Mesh> meshFromStream(std::istream &in) {
   std::vector<tinyobj::shape_t> shapes;
   std::vector<tinyobj::material_t> materials;
   tinyobj::NullMaterialReader reader;

   std::string error = tinyobj::LoadObj(shapes, materials, in, reader);

   if (!error.empty()) {
      LOG_WARNING("Unable to load mesh, reverting to empty: " << error);
      return std::make_shared<Mesh>();
   }

   if (shapes.empty()) {
      LOG_WARNING("No shapes while loading mesh, reverting to empty");
      return std::make_shared<Mesh>();
   }

   const tinyobj::shape_t &shape = shapes.at(0);
   unsigned int dimensionality = 3;

   unsigned int numVertices = shape.mesh.positions.size() / dimensionality;
   unsigned int numNormals = shape.mesh.normals.size() / dimensionality;
   unsigned int numTexCoords = shape.mesh.texcoords.size() / 2;
   unsigned int numIndices = shape.mesh.indices.size();

   return std::make_shared<Mesh>(shape.mesh.positions.data(), numVertices, shape.mesh.normals.data(), numNormals,
                                 shape.mesh.texcoords.data(), numTexCoords, shape.mesh.indices.data(), numIndices,
                                 dimensionality);
}

SPtr<Mesh> getMeshFromMemory(const char *data) {
   std::stringstream ss(data);
   return meshFromStream(ss);
}

} // namespace

SPtr<Mesh> MeshLoader::loadMesh(const std::string &fileName) {
   MeshMap::iterator location(meshMap.find(fileName));
   if (location != meshMap.end()) {
      return location->second;
   }

   SPtr<Mesh> mesh;

   if (!IOUtils::canRead(fileName)) {
      LOG_WARNING("Unable to load mesh from file \"" << fileName << "\", reverting to default");
      mesh = getMeshForShape(MeshShape::Cube);
   }

   std::ifstream in(fileName);
   mesh = meshFromStream(in);

   if (!mesh) {
      LOG_WARNING("Unable to import mesh \"" << fileName << "\", reverting to default");
      mesh = getMeshForShape(MeshShape::Cube);
   }

   meshMap.insert({ fileName, mesh });
   return mesh;
}

SPtr<Mesh> MeshLoader::getMeshForShape(MeshShape shape) {
   switch (shape) {
      case MeshShape::Cube:
         if (!cubeMesh) {
            cubeMesh = getMeshFromMemory(kCubeMeshSource);
         }
         return cubeMesh;
      case MeshShape::XYPlane:
         if (!xyPlaneMesh) {
            xyPlaneMesh = getMeshFromMemory(kXyPlaneMeshSource);
         }
         return xyPlaneMesh;
      default:
         ASSERT(false, "Invalid mesh shape");
         return nullptr;
   }
}

} // namespace Shiny
