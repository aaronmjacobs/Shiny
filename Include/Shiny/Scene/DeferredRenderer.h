#ifndef SHINY_DEFERRED_RENDERER_H
#define SHINY_DEFERRED_RENDERER_H

#include "Shiny/Pointers.h"
#include "Shiny/Graphics/Model.h"
#include "Shiny/Scene/Renderer.h"

namespace Shiny {

class Framebuffer;
class ShaderLoader;

class DeferredRenderer : public Renderer {
public:
   DeferredRenderer(ShaderLoader& shaderLoader);

   virtual void render(const Scene& scene) override;

private:
   Model screenQuad;
   UPtr<Framebuffer> framebuffer;
};

} // namespace Shiny

#endif
