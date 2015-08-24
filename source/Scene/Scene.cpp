#include "Scene.h"

#include "Camera.h"
#include "SceneObject.h"
#include "ShinyAssert.h"

#include <algorithm>

namespace Shiny {

namespace {

void removeFromProgramMap(const SPtr<SceneObject> &object, const SPtr<ShaderProgram> &program,
                          SceneObjectProgramMap &objectsByProgram) {
   SceneObjectProgramMap::iterator mapLoc(objectsByProgram.find(program));
   ASSERT(mapLoc != objectsByProgram.end(), "Shader program does not exist in map");

   SceneObjectVector &vec(mapLoc->second);
   SceneObjectVector::iterator vecLoc(std::find(vec.begin(), vec.end(), object));
   ASSERT(vecLoc != vec.end(), "Object does not exist in shader program vector");

   vec.erase(vecLoc);
}

} // namespace

// static
SPtr<Scene> Scene::create() {
   return SPtr<Scene>(new Scene);
}

Scene::Scene() {
   cameras.push_back(std::make_shared<Camera>());
}

Scene::~Scene() {
}

SPtr<SceneObject> Scene::createObject() {
   SPtr<SceneObject> object(new SceneObject(shared_from_this()));

   objects.push_back(object);

   return object;
}

void Scene::removeObject(const SPtr<SceneObject> &object) {
   SceneObjectVector::iterator vecLoc(std::find(objects.begin(), objects.end(), object));
   ASSERT(vecLoc != objects.end(), "Object does not exist in vector");
   objects.erase(vecLoc);

   const SPtr<ShaderProgram> &program(object->getModel().getProgram());
   if (program) {
      removeFromProgramMap(object, program, objectsByProgram);
   }
}

void Scene::shaderProgramChange(const SPtr<SceneObject> &object, const SPtr<ShaderProgram> &oldProgram,
                                const SPtr<ShaderProgram> &newProgram) {
   ASSERT(object, "Trying to change shader program for null object");

   if (oldProgram == newProgram) {
      return;
   }

   if (oldProgram != nullptr) {
      removeFromProgramMap(object, oldProgram, objectsByProgram);
   }

   if (newProgram != nullptr) {
      std::pair<SceneObjectProgramMap::iterator, bool> newMapLoc(objectsByProgram.emplace(newProgram,
                                                                                          SceneObjectVector()));
      newMapLoc.first->second.push_back(object);
   }
}

SPtr<Camera> Scene::createCamera() {
   SPtr<Camera> camera(std::make_shared<Camera>());

   cameras.push_back(camera);

   return camera;
}

void Scene::removeCamera(const SPtr<Camera> &camera) {
   ASSERT(cameras.size() > 1, "Trying to remove last camera from scene");

   CameraVector::iterator loc(std::find(cameras.begin(), cameras.end(), camera));
   ASSERT(loc != cameras.end(), "Camera does not exist in vector");

   cameras.erase(loc);
}

const SceneObjectVector& Scene::getObjects() const {
   return objects;
}

const SceneObjectProgramMap& Scene::getObjectsByProgram() const {
   return objectsByProgram;
}

const SPtr<Camera>& Scene::getDefaultCamera() {
   return cameras.at(0);
}

} // namespace Shiny
