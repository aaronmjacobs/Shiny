#ifndef SHINY_MODEL_COMPONENT_H
#define SHINY_MODEL_COMPONENT_H

#include "Shiny/Graphics/Model.h"
#include "Shiny/Scene/TransformComponent.h"

namespace Shiny {

class ModelComponent : public TransformComponent {
public:
   using OnShaderProgramChangeDelegate = Delegate<void, ModelComponent*, ShaderProgram*>;

   void render(RenderData renderData);

   OnShaderProgramChangeDelegate::Handle bindOnShaderProgramChange(const OnShaderProgramChangeDelegate::FuncType& function) {
      return onShaderProgramChange.bind(function);
   }

   const SPtr<Mesh>& getMesh() const {
      return model.getMesh();
   }

   void setMesh(const SPtr<Mesh>& mesh) {
      model.setMesh(mesh);
   }

   const SPtr<ShaderProgram>& getShaderProgram() const {
      return model.getShaderProgram();
   }

   void setShaderProgram(const SPtr<ShaderProgram>& program) {
      onShaderProgramChange.execute(this, program.get());
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
   OnShaderProgramChangeDelegate onShaderProgramChange;
};

SHINY_REFERENCE_COMPONENT(ModelComponent)

} // namespace Shiny

#endif
