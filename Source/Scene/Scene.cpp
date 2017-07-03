#include "Shiny/Scene/ModelComponent.h"
#include "Shiny/Scene/Scene.h"

namespace Shiny {

void Scene::registerModelComponent(ModelComponent* modelComponent) {
   modelDestroyHandles.push_back(modelComponent->bindOnDestroy([this](Component* component) {
      for (auto& pair : modelComponentsByShaderProgram) {
         auto itr = std::find(pair.second.begin(), pair.second.end(), static_cast<ModelComponent*>(component));
         if (itr != pair.second.end()) {
            pair.second.erase(itr);
            // TODO Destroy map entry
            return;
         }
      }
   }));

   shaderProgramChangedHandles.push_back(modelComponent->bindOnShaderProgramChanged([this](ModelComponent* modelComponent, ShaderProgram* oldProgram, ShaderProgram* newProgram) {
      auto mapItr = modelComponentsByShaderProgram.find(oldProgram);
      ASSERT(mapItr != modelComponentsByShaderProgram.end());

      auto vecItr = std::find(mapItr->second.begin(), mapItr->second.end(), modelComponent);
      ASSERT(vecItr != mapItr->second.end());

      mapItr->second.erase(vecItr);
      if (mapItr->second.empty()) {
         modelComponentsByShaderProgram.erase(mapItr);
      }

      auto pair = modelComponentsByShaderProgram.emplace(newProgram, std::vector<ModelComponent*>());
      pair.first->second.push_back(modelComponent);
   }));

   ShaderProgram* shaderProgram = modelComponent->getShaderProgram().get();
   auto pair = modelComponentsByShaderProgram.emplace(shaderProgram, std::vector<ModelComponent*>());
   pair.first->second.push_back(modelComponent);
}

} // namespace Shiny
