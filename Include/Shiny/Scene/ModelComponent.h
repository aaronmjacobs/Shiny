#ifndef SHINY_MODEL_COMPONENT_H
#define SHINY_MODEL_COMPONENT_H

#include "Shiny/Graphics/Model.h"
#include "Shiny/Scene/TransformComponent.h"

namespace Shiny {

class ModelComponent : public TransformComponent {
public:
   using OnShaderProgramChangedDelegate = Delegate<void, ModelComponent*, ShaderProgram*, ShaderProgram*>;

   void render(RenderData renderData);

   OnShaderProgramChangedDelegate::Handle bindOnShaderProgramChanged(const OnShaderProgramChangedDelegate::FuncType& function) {
      return onShaderProgramChanged.bind(function);
   }

   const SPtr<Mesh>& getMesh() const {
      return model.getMesh();
   }

   const SPtr<ShaderProgram>& getShaderProgram() const {
      return model.getShaderProgram();
   }

   void setMesh(const SPtr<Mesh>& mesh) {
      model.setMesh(mesh);
   }

   void setShaderProgram(const SPtr<ShaderProgram>& program) {
      onShaderProgramChanged.execute(this, getShaderProgram().get(), program.get());
      model.setShaderProgram(program);
   }

   void attachMaterial(const SPtr<Material>& material) {
      model.attachMaterial(material);
   }

   void removeMaterial(const SPtr<Material>& material) {
      model.removeMaterial(material);
   }

protected:
   friend class ComponentRegistrar<ModelComponent>;

   ModelComponent(Entity& entity);

private:
   Model model;
   OnShaderProgramChangedDelegate onShaderProgramChanged;
};

SHINY_REFERENCE_COMPONENT(ModelComponent)

} // namespace Shiny

#endif
