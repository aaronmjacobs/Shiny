#ifndef SHINY_RENDERER_H
#define SHINY_RENDERER_H

namespace Shiny {

class Scene;

class Renderer {
public:
   virtual ~Renderer() = default;

   virtual void render(const Scene& scene) = 0;
};

} // namespace Shiny

#endif
