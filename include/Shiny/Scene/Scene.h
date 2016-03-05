#ifndef SHINY_SCENE_H
#define SHINY_SCENE_H

#include "Shiny/Pointers.h"

#include <map>
#include <vector>

namespace Shiny {

class Camera;
class SceneObject;
class ShaderProgram;

typedef std::vector<SPtr<SceneObject>> SceneObjectVector;
typedef std::map<SPtr<ShaderProgram>, SceneObjectVector> SceneObjectProgramMap;
typedef std::vector<SPtr<Camera>> CameraVector;

class Scene : public std::enable_shared_from_this<Scene> {
public:
   static SPtr<Scene> create();

private:
   Scene();

protected:
   SceneObjectVector objects;
   SceneObjectProgramMap objectsByProgram;
   CameraVector cameras;

public:
   virtual ~Scene() = default;

   SPtr<SceneObject> createObject();

   void removeObject(const SPtr<SceneObject> &object);

   void shaderProgramChange(const SPtr<SceneObject> &object, const SPtr<ShaderProgram> &oldProgram,
                            const SPtr<ShaderProgram> &newProgram);

   SPtr<Camera> createCamera();

   void removeCamera(const SPtr<Camera> &camera);

   const SceneObjectVector& getObjects() const;

   const SceneObjectProgramMap& getObjectsByProgram() const;

   const SPtr<Camera>& getDefaultCamera();
};

} // namespace Shiny

#endif
