#ifndef SHINY_FRAMEBUFFER_H
#define SHINY_FRAMEBUFFER_H

#include "Shiny/Pointers.h"

#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Graphics/Viewport.h"

namespace Shiny {

class Texture;

class Framebuffer {
protected:
   /**
    * Framebuffer object
    */
   GLuint framebuffer { 0 };

   /**
    * Color attachment texture
    */
   SPtr<Texture> texture;

   /**
    * Depth attachment texture
    */
   SPtr<Texture> depthTexture;

   /**
    * Resolution of the textures
    */
   int width { 0 };
   int height { 0 };

   /**
    * If the framebuffer has been initialized
    */
   bool initialized { false };

   /**
    * The last viewport used before this framebuffer was used
    */
   Viewport lastViewport {};

   void release();

   void move(Framebuffer &&other);

public:
   Framebuffer();

   Framebuffer(Framebuffer &&other);

   Framebuffer& operator=(Framebuffer &&other);

   ~Framebuffer();

   /**
    * Initializes the textures to the given resolution; may be called multiple times
    */
   bool init(int width, int height);

   /**
    * Initializes the textures to the resolution of the viewport; may be called multiple times
    */
   bool init();

   /**
    * Causes all drawing to occur in the framebuffer
    */
   void use();

   /**
    * Causes all drawing to occur in the back buffer
    */
   void disable() const;

   /**
    * Returns the texture of the color attachment
    */
   SPtr<Texture> getTexture() const;

   /**
    * Returns the texture of the depth attachment
    */
   SPtr<Texture> getDepthTexture() const;
};

} // namespace Shiny

#endif
