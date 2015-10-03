set(HEADER_NAMES
   Defines.h
   Engine.h
   Log.h
   Pointers.h
   Shiny.h
   ShinyAssert.h
   Version.h
   Assets/DefaultImageSource.h
   Assets/MeshLoader.h
   Assets/ShaderLoader.h
   Assets/TextureLoader.h
   Audio/AudioSystem.h
   Graphics/Context.h
   Graphics/Framebuffer.h
   Graphics/Material.h
   Graphics/Mesh.h
   Graphics/Model.h
   Graphics/OpenGL.h
   Graphics/RenderData.h
   Graphics/Shader.h
   Graphics/ShaderProgram.h
   Graphics/Texture.h
   Graphics/TextureMaterial.h
   Graphics/Viewport.h
   Input/Controller.h
   Input/ControllerMap.h
   Input/InputDevice.h
   Input/Keyboard.h
   Input/Keys.h
   Input/Mouse.h
   Platform/IOUtils.h
   Platform/OSUtils.h
   Scene/Camera.h
   Scene/Scene.h
   Scene/SceneObject.h
   Scene/Transform.h
   Text/Font.h
   Text/FontAtlas.h
   Text/TextRenderer.h
)
source_group("Includes" "(${INCLUDE_DIR}|${BIN_INCLUDE_DIR})/${PROJECT_NAME}/*")
source_group("Includes\\Assets" "${INCLUDE_DIR}/${PROJECT_NAME}/Assets/*")
source_group("Includes\\Audio" "${INCLUDE_DIR}/${PROJECT_NAME}/Audio/*")
source_group("Includes\\Graphics" "${INCLUDE_DIR}/${PROJECT_NAME}/Graphics/*")
source_group("Includes\\Input" "${INCLUDE_DIR}/${PROJECT_NAME}/Input/*")
source_group("Includes\\Platform" "${INCLUDE_DIR}/${PROJECT_NAME}/Platform/*")
source_group("Includes\\Scene" "${INCLUDE_DIR}/${PROJECT_NAME}/Scene/*")
source_group("Includes\\Text" "${INCLUDE_DIR}/${PROJECT_NAME}/Text/*")