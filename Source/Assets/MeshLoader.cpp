#include "Shiny/Log.h"
#include "Shiny/ShinyAssert.h"
#include "Shiny/Assets/MeshLoader.h"
#include "Shiny/Graphics/Mesh.h"
#include "Shiny/Platform/IOUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader.h>

#include <cstring>
#include <map>
#include <vector>

namespace tinyobj {

class NullMaterialReader : public MaterialReader {
public:
   NullMaterialReader() {}
   virtual ~NullMaterialReader() {}
   virtual bool operator()(const std::string &matId,
                           std::vector<material_t> *materials,
                           std::map<std::string, int> *matMap,
                           std::string *err) override {
      return true;
   }
};

} // namespace tinyobj

namespace Shiny {

namespace {

// TODO Bake in as arrays of each data type

const char* kCubeMeshSource = "v -0.500000 -0.500000 0.500000\nv 0.500000 -0.500000 0.500000\nv -0.500000 0.500000 0.500000\nv 0.500000 0.500000 0.500000\nv -0.500000 0.500000 -0.500000\nv 0.500000 0.500000 -0.500000\nv -0.500000 -0.500000 -0.500000\nv 0.500000 -0.500000 -0.500000\n\nvt 0.000000 0.000000\nvt 1.000000 0.000000\nvt 0.000000 1.000000\nvt 1.000000 1.000000\n\nvn 0.000000 0.000000 1.000000\nvn 0.000000 1.000000 0.000000\nvn 0.000000 0.000000 -1.000000\nvn 0.000000 -1.000000 0.000000\nvn 1.000000 0.000000 0.000000\nvn -1.000000 0.000000 0.000000\n\ns 1\nf 1/1/1 2/2/1 3/3/1\nf 3/3/1 2/2/1 4/4/1\ns 2\nf 3/1/2 4/2/2 5/3/2\nf 5/3/2 4/2/2 6/4/2\ns 3\nf 5/4/3 6/3/3 7/2/3\nf 7/2/3 6/3/3 8/1/3\ns 4\nf 7/1/4 8/2/4 1/3/4\nf 1/3/4 8/2/4 2/4/4\ns 5\nf 2/1/5 8/2/5 4/3/5\nf 4/3/5 8/2/5 6/4/5\ns 6\nf 7/1/6 1/2/6 5/3/6\nf 5/3/6 1/2/6 3/4/6\n";

const char* kXyPlaneMeshSource = "v -1.000000 -1.000000 -0.000000\nv 1.000000 -1.000000 -0.000000\nv -1.000000 1.000000 0.000000\nv 1.000000 1.000000 0.000000\nvt 0.000000 0.000000\nvt 1.000000 0.000000\nvt 0.000000 1.000000\nvt 1.000000 1.000000\nvn 0.000000 -0.000000 1.000000\ns off\nf 2/2/1 4/4/1 3/3/1\nf 1/1/1 2/2/1 3/3/1\n";

std::vector<glm::vec3> generateNormals(const float* vertices, unsigned int numVertices, const unsigned int* indices, unsigned int numIndices) {
   std::vector<glm::vec3> calcedNormals(numVertices);

   for (std::size_t i = 0; i < numIndices / 3; ++i) {
      unsigned int index1 = indices[i * 3];
      unsigned int index2 = indices[i * 3 + 1];
      unsigned int index3 = indices[i * 3 + 2];

      glm::vec3 vertex1(vertices[index1 * 3], vertices[index1 * 3 + 1], vertices[index1 * 3 + 2]);
      glm::vec3 vertex2(vertices[index2 * 3], vertices[index2 * 3 + 1], vertices[index2 * 3 + 2]);
      glm::vec3 vertex3(vertices[index3 * 3], vertices[index3 * 3 + 1], vertices[index3 * 3 + 2]);

      glm::vec3 side1 = vertex2 - vertex1;
      glm::vec3 side2 = vertex3 - vertex1;

      glm::vec3 normal = glm::normalize(glm::cross(side1, side2));
      calcedNormals[index1] += normal;
      calcedNormals[index2] += normal;
      calcedNormals[index3] += normal;
   }

   for (glm::vec3& calcedNormal : calcedNormals) {
      if (glm::length(calcedNormal) > 0.0f) {
         calcedNormal = glm::normalize(calcedNormal);
      }
   }

   return calcedNormals;
}

SPtr<Mesh> meshFromStream(std::istream& in, bool generateNormalsIfMissing) {
   tinyobj::attrib_t attributes;
   std::vector<tinyobj::shape_t> shapes;
   std::vector<tinyobj::material_t> materials;
   tinyobj::NullMaterialReader reader;

   std::string errorMessage;
   bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &errorMessage, &in, &reader, true);

   if (!success) {
      LOG_WARNING("Unable to load mesh, reverting to empty: " << errorMessage);
      return std::make_shared<Mesh>();
   }

   if (shapes.empty()) {
      LOG_WARNING("No shapes while loading mesh, reverting to empty");
      return std::make_shared<Mesh>();
   }

   const tinyobj::shape_t &shape = shapes.at(0);
   unsigned int dimensionality = 3;

   unsigned int numVertices = static_cast<unsigned int>(attributes.vertices.size() / dimensionality);
   unsigned int numNormals = static_cast<unsigned int>(attributes.normals.size() / dimensionality);
   unsigned int numTexCoords = static_cast<unsigned int>(attributes.texcoords.size() / 2);
   unsigned int numIndices = static_cast<unsigned int>(shape.mesh.indices.size());

   std::vector<unsigned int> indices(numIndices);
   for (std::size_t i = 0; i < numIndices; ++i) {
      indices[i] = shape.mesh.indices[i].vertex_index;
   }

   const float* normals = attributes.normals.data();
   std::vector<glm::vec3> generatedNormals;
   if (numNormals == 0 && generateNormalsIfMissing) {
      generatedNormals = generateNormals(attributes.vertices.data(), numVertices, indices.data(), numIndices);
      normals = glm::value_ptr(*generatedNormals.data());
      numNormals = static_cast<unsigned int>(generatedNormals.size());
   }

   return std::make_shared<Mesh>(attributes.vertices.data(), numVertices, normals, numNormals,
                                 attributes.texcoords.data(), numTexCoords, indices.data(), numIndices,
                                 dimensionality);
}

SPtr<Mesh> getMeshFromMemory(const char* data) {
   std::stringstream ss(data);
   return meshFromStream(ss, true);
}

} // namespace

SPtr<Mesh> MeshLoader::loadMesh(const Path& filePath, bool generateNormalsIfMissing) {
   auto location = meshMap.find(filePath);
   if (location != meshMap.end()) {
      return location->second;
   }

   SPtr<Mesh> mesh;

   if (!IOUtils::canRead(filePath)) {
      LOG_WARNING("Unable to load mesh from file \"" << filePath << "\", reverting to default");
      mesh = getMeshForShape(MeshShape::Cube);
   }

   std::ifstream in(filePath.toString());
   mesh = meshFromStream(in, generateNormalsIfMissing);

   if (!mesh) {
      LOG_WARNING("Unable to import mesh \"" << filePath << "\", reverting to default");
      mesh = getMeshForShape(MeshShape::Cube);
   }

   meshMap.insert({ filePath, mesh });
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
