#ifndef SHINY_FRAMEBUFFER_H
#define SHINY_FRAMEBUFFER_H

#include "Shiny/Pointers.h"

#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/TextureInfo.h"
#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Graphics/Viewport.h"

#include <vector>

namespace Shiny {

class Texture;

class Framebuffer {
public:
   static void bindDefaultFramebuffer() {
      Context::current()->bindFramebuffer(0);
   }

   Framebuffer(GLsizei attachmentWidth = 0, GLsizei attachmentHeight = 0, bool withDepthStencil = true,
               const std::vector<Tex::InternalFormat>& attachmentFormats = { Tex::InternalFormat::kRGB8 });

   Framebuffer(const Framebuffer& other) = delete;
   Framebuffer(Framebuffer&& other) {
      move(std::move(other));
   }

   ~Framebuffer() {
      release();
   }

   Framebuffer& operator=(const Framebuffer& other) = delete;
   Framebuffer& operator=(Framebuffer&& other) {
      release();
      move(std::move(other));
      return *this;
   }

   void reset(GLsizei attachmentWidth, GLsizei attachmentHeight, bool withDepthStencil,
              const std::vector<Tex::InternalFormat>& attachmentFormats);

   void setResolution(GLsizei attachmentWidth, GLsizei attachmentHeight);

   /**
    * Causes all drawing to occur in the framebuffer
    */
   void bind();

   /**
    * Returns the texture of the color attachment
    */
   const SPtr<Texture>& getColorAttachment(std::size_t index) const {
      ASSERT(index < colorAttachments.size());
      return colorAttachments[index];
   }

   /**
    * Returns the texture of the depth / stencil attachment
    */
   const SPtr<Texture>& getDepthStencilAttachment() const {
      return depthStencilAttachment;
   }

   std::size_t getNumColorAttachments() const {
      return colorAttachments.size();
   }

   bool hasDepthStencilAttachment() const {
      return depthStencilAttachment != nullptr;
   }

private:
   void release();
   void move(Framebuffer&& other);

   /**
   * Framebuffer object
   */
   GLuint fbo;

   /**
   * Depth attachment texture
   */
   SPtr<Texture> depthStencilAttachment;

   /**
   * Color attachment texture
   */
   std::vector<SPtr<Texture>> colorAttachments;

   /**
   * Resolution of the textures
   */
   GLsizei width;
   GLsizei height;
};

} // namespace Shiny

#endif
