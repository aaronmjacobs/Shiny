set(SOURCE_NAMES
   Engine.cpp
   Shiny.cpp
   Assets/AudioLoader.cpp
   Assets/MeshLoader.cpp
   Assets/ShaderLoader.cpp
   Assets/TextureLoader.cpp
   Audio/AudioBuffer.cpp
   Audio/AudioSource.cpp
   Audio/AudioSystem.cpp
   Audio/Sound.cpp
   Audio/Stream.cpp
   Graphics/Context.cpp
   Graphics/Framebuffer.cpp
   Graphics/Mesh.cpp
   Graphics/Model.cpp
   Graphics/RenderData.cpp
   Graphics/Shader.cpp
   Graphics/ShaderProgram.cpp
   Graphics/Texture.cpp
   Graphics/TextureMaterial.cpp
   Input/Controller.cpp
   Input/ControllerMap.cpp
   Input/Keyboard.cpp
   Input/Mouse.cpp
   Platform/IOUtils.cpp
   Platform/OSUtils.cpp
   Scene/Camera.cpp
   Scene/Scene.cpp
   Scene/SceneObject.cpp
   Text/Font.cpp
   Text/FontAtlas.cpp
   Text/TextRenderer.cpp
)
source_group("Sources" "${SRC_DIR}/*")
source_group("Sources\\Assets" "${SRC_DIR}/Assets/*")
source_group("Sources\\Audio" "${SRC_DIR}/Audio/*")
source_group("Sources\\Graphics" "${SRC_DIR}/Graphics/*")
source_group("Sources\\Input" "${SRC_DIR}/Input/*")
source_group("Sources\\Platform" "${SRC_DIR}/Platform/*")
source_group("Sources\\Scene" "${SRC_DIR}/Scene/*")
source_group("Sources\\Text" "${SRC_DIR}/Text/*")