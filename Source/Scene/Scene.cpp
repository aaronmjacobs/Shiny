#include "Shiny/Scene/ModelComponent.h"
#include "Shiny/Scene/Scene.h"

namespace Shiny {

void Scene::registerModelComponent(ModelComponent* modelComponent) {
   auto addToMap = [this](ModelComponent* modelComponent, ShaderProgram* newProgram) {
      auto pair = modelComponentsByShaderProgram.emplace(newProgram, std::vector<ModelComponent*>());
      pair.first->second.push_back(modelComponent);
   };

   auto removeFromMap = [this](ModelComponent* modelComponent) {
      ShaderProgram* oldProgram = modelComponent->getShaderProgram().get();

      auto mapItr = modelComponentsByShaderProgram.find(oldProgram);
      ASSERT(mapItr != modelComponentsByShaderProgram.end());

      auto vecItr = std::find(mapItr->second.begin(), mapItr->second.end(), modelComponent);
      ASSERT(vecItr != mapItr->second.end());

      mapItr->second.erase(vecItr);
      if (mapItr->second.empty()) {
         modelComponentsByShaderProgram.erase(mapItr);
      }
   };

   ModelComponentHandles handles;
   handles.onDestroyHandle = modelComponent->bindOnDestroy([this, removeFromMap](Component* component) {
      ModelComponent* modelComponent = static_cast<ModelComponent*>(component);
      removeFromMap(modelComponent);
      modelComponentHandles.erase(modelComponent);
   });
   handles.onShaderProgramChangeHandle = modelComponent->bindOnShaderProgramChange(
      [this, removeFromMap, addToMap](ModelComponent* modelComponent, ShaderProgram* newProgram) {
      removeFromMap(modelComponent);
      addToMap(modelComponent, newProgram);
   });

   modelComponentHandles[modelComponent] = std::move(handles);
   addToMap(modelComponent, modelComponent->getShaderProgram().get());
}

} // namespace Shiny
