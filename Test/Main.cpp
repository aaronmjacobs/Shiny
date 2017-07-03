#include <Shiny/Engine.h>
#include <Shiny/Log.h>
#include <Shiny/Shiny.h>
#include <Shiny/Assets/MeshLoader.h>
#include <Shiny/Assets/ShaderLoader.h>
#include <Shiny/Assets/TextureLoader.h>
#include <Shiny/Entity/Component.h>
#include <Shiny/Entity/Delegate.h>
#include <Shiny/Entity/Entity.h>
#include <Shiny/Graphics/Framebuffer.h>
#include <Shiny/Graphics/Mesh.h>
#include <Shiny/Graphics/Model.h>
#include <Shiny/Graphics/ShaderProgram.h>
#include <Shiny/Graphics/TextureMaterial.h>
#include <Shiny/Input/Keyboard.h>
#include <Shiny/Input/Mouse.h>
#include <Shiny/Scene/CameraComponent.h>
#include <Shiny/Scene/DeferredRenderer.h>
#include <Shiny/Scene/ForwardRenderer.h>
#include <Shiny/Scene/ModelComponent.h>
#include <Shiny/Scene/Scene.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdio>

namespace {

using namespace Shiny;

class TestEngine : public Engine {
public:
   void load();

   virtual void tick(const float dt) override;

   virtual void render() override;

private:
   MeshLoader meshLoader;
   ShaderLoader shaderLoader;
   TextureLoader textureLoader;
   Scene scene;

   Entity* camera;
   Entity* ogre;

   UPtr<ForwardRenderer> forwardRenderer;
   UPtr<DeferredRenderer> deferredRenderer;
   bool useDeferredRenderer = false;
};

void TestEngine::load() {
   glEnable(GL_DEPTH_TEST);

   camera = scene.createEntity<CameraComponent>();
   CameraComponent* cameraComponent = camera->getComponentByClass<CameraComponent>();
   scene.setActiveCamera(cameraComponent);

   /*ModelComponent* testModelComponent = camera->getComponentByClass<ModelComponent>();
   auto handle = testModelComponent->bindOnDestroy([](Component* component) { LOG_INFO("On component destroyed"); });
   camera->destroy();*/

   ogre = scene.createEntity<ModelComponent>();
   ModelComponent* modelComponent = ogre->getComponentByClass<ModelComponent>();
   modelComponent->setMesh(meshLoader.loadMesh(Shiny::dataPath() + "/bunny.obj"));
   modelComponent->attachMaterial(std::make_shared<TextureMaterial>(textureLoader.loadTexture(Shiny::dataPath() + "/ogre/diffuse.png"), "diffuseTexture"));
   modelComponent->attachMaterial(std::make_shared<TextureMaterial>(textureLoader.loadTexture(Shiny::dataPath() + "/ogre/bump.png"), "specularTexture"));
   modelComponent->attachMaterial(std::make_shared<TextureMaterial>(textureLoader.loadTexture(Shiny::dataPath() + "/ogre/normalmap.png"), "normalTexture"));
   modelComponent->setShaderProgram(shaderLoader.loadShaderProgram(Shiny::dataPath() + "/GBuffer"));

   const Entity& constOgre = *ogre;
   const ModelComponent* derp = constOgre.getComponentByClass<ModelComponent>();
   std::vector<const ModelComponent*> herp = constOgre.getComponentsByClass<ModelComponent>();

   Transform& transform = modelComponent->getRelativeTransform();
   transform.position = glm::vec3(0.0f, 0.0f, -2.0f);

   /*SPtr<SceneObject> cube = scene->createObject();
   cube->setMesh(meshLoader.getMeshForShape(MeshShape::Cube));
   cube->setShaderProgram(shaderLoader.loadShaderProgram(Shiny::dataPath() + "/Cubemap"));
   cube->attachMaterial(std::make_shared<TextureMaterial>(textureLoader.loadCubemap(Shiny::dataPath() + "/arches", "png"), "uTexture"));*/

   forwardRenderer = std::make_unique<ForwardRenderer>();
   deferredRenderer = std::make_unique<DeferredRenderer>(shaderLoader);
}

void TestEngine::tick(const float dt) {
   static const float kMouseSensitivity = 0.3f;
   static const float kMovementSpeed = 2.0f;

   if (keyboard->pressed(Key::kEscape)) {
      glfwSetWindowShouldClose(window.get(), 1);
   }

   if (keyboard->pressed(Key::kSpace, true)) {
      useDeferredRenderer = !useDeferredRenderer;
   }

   CameraComponent* cameraComponent = scene.getActiveCamera();
   if (!cameraComponent) {
      return;
   }

   cameraComponent->rotate(static_cast<float>(mouse->yDiff() * dt * kMouseSensitivity), static_cast<float>(mouse->xDiff() * dt * kMouseSensitivity));

   glm::vec3 movement(0.0f);
   if (keyboard->pressed(Key::kW)) {
      movement += cameraComponent->getFront();
   }
   if (keyboard->pressed(Key::kS)) {
      movement -= cameraComponent->getFront();
   }
   if (keyboard->pressed(Key::kD)) {
      movement += cameraComponent->getRight();
   }
   if (keyboard->pressed(Key::kA)) {
      movement -= cameraComponent->getRight();
   }
   if (glm::length(movement) > 0.0f) {
      movement = glm::normalize(movement);
   }
   movement *= dt * kMovementSpeed;

   Transform& cameraTransform = cameraComponent->getRelativeTransform();
   cameraTransform.position += movement;
}

void TestEngine::render() {
   if (useDeferredRenderer) {
      deferredRenderer->render(scene);
   } else {
      forwardRenderer->render(scene);
   }
}

class DerpComponent : public Component {
public:
   virtual ~DerpComponent() {
      LOG_INFO("Herp");
   }

   virtual void onOwnerInitialized() override {
      LOG_INFO("Owner initialized");
   }

   virtual void onComponentAddedToOwner(Component* component) override {
      LOG_INFO("Component added: " << component);
   }

protected:
   friend class ComponentRegistrar<DerpComponent>;

   DerpComponent(Entity& entity)
      : Component(entity) {
   }
};

SHINY_REGISTER_COMPONENT(DerpComponent)

} // namespace

int main(int argc, char* argv[]) {
   Delegate<void, int, float> testDelegate;
   Delegate<void, int, float>::Handle handle = testDelegate.bind([](int x, float y) { LOG_INFO("x: " << x << ", y: " << y); });
   testDelegate.execute(42, 65.2f);

   using MyDelegate = Delegate<int>;

   MyDelegate test2;
   {
      MyDelegate::Handle first = test2.bind([]() { return 42; });
      MyDelegate::Handle second = test2.bind([]() { return 36; });
      MyDelegate::Handle third = test2.bind([]() { return 79; });
      second = nullptr;
      std::vector<MyDelegate::ReturnType> values = test2.executeWithReturn();
      for (int value : values) {
         LOG_INFO("Value: " << value);
      }
   }

   /*
   {
      UPtr<Entity> entity = Entity::create<Entity, SceneComponent, DerpComponent, DerpComponent>();
      SPtr<Entity> entity2 = Entity::createShared<Entity, DerpComponent, DerpComponent>();
      //entity.createComponent<DerpComponent>(42);

      Component* test = entity->createComponentByName("DerpComponent");
      SceneComponent* sceneComponent = entity->getComponentByClass<SceneComponent>();

      DerpComponent* derpComponent = entity->getComponentByClass<DerpComponent>();
      std::vector<DerpComponent*> derpComponents = entity->getComponentsByClass<DerpComponent>();
      int x = 0;
   }

   {
      SPtr<Scene> scene = Scene::create();
      UPtr<Entity> testSceneObject = Entity::create<Entity, SceneComponent, DerpComponent>(*scene);
      UPtr<Entity> testSceneObject2 = Entity::create<Entity>({ "DerpComponent", "SceneComponent" }, *scene);
      SPtr<Entity> testSceneObject3 = Entity::createShared<Entity, SceneComponent, DerpComponent>(*scene);
      SPtr<Entity> testSceneObject4 = Entity::createShared<Entity>({ "DerpComponent", "SceneComponent" }, *scene);
      //glm::vec3 position = testSceneObject->getPosition();
      //position.x = 42.0f;
   }
   */

   /*std::vector<MyDelegate::ReturnType> results = test2.executeWithReturn();
   for (int result : results) {
      LOG_INFO("result: " << result);
   }*/

   Result initResult = init(argc, argv);
   if (initResult != Result::kOK) {
      LOG_ERROR_MSG_BOX(errorString(initResult));
      return 1;
   }

   int retVal = 0;
   {
      TestEngine engine;
      Engine::Result startResult = engine.startUp(1280, 720, "Shiny Test");
      if (startResult == Engine::Result::kOK) {
         engine.load();
         engine.run();
      } else {
         LOG_ERROR_MSG_BOX(Engine::errorString(startResult));
         retVal = 1;
      }
   }

   return retVal;
}
