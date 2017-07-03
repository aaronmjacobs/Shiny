#ifndef SHINY_FORWARD_RENDERER_H
#define SHINY_FORWARD_RENDERER_H

#include "Shiny/Scene/Renderer.h"

namespace Shiny {

class ForwardRenderer : public Renderer {
public:
   virtual void render(const Scene& scene) override;
};

} // namespace Shiny

#endif
