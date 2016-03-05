#ifndef SHINY_VIEWPORT_H
#define SHINY_VIEWPORT_H

namespace Shiny {

struct Viewport {
   int x { 0 };
   int y { 0 };
   int width { 0 };
   int height { 0 };

   Viewport() = default;

   Viewport(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
};

} // namespace Shiny

#endif
