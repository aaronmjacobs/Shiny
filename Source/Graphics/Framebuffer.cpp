#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Context.h"
#include "Shiny/Graphics/Framebuffer.h"
#include "Shiny/Graphics/Texture.h"

namespace Shiny {

Framebuffer::Framebuffer(GLsizei attachmentWidth, GLsizei attachmentHeight, bool withDepthStencil, const std::vector<Tex::InternalFormat>& attachmentFormats)
   : fbo(0), width(0), height(0) {
   glGenFramebuffers(1, &fbo);

   reset(attachmentWidth, attachmentHeight, withDepthStencil, attachmentFormats);
}

void Framebuffer::release() {
   if (fbo) {
      Context::current()->onFramebufferDeleted(fbo);
      glDeleteFramebuffers(1, &fbo);
      fbo = 0;
   }
}

void Framebuffer::move(Framebuffer&& other) {
   fbo = other.fbo;
   depthStencilAttachment = std::move(other.depthStencilAttachment);
   colorAttachments = std::move(other.colorAttachments);
   width = other.width;
   height = other.height;

   other.fbo = 0;
   other.width = 0;
   other.height = 0;
}

void Framebuffer::reset(GLsizei attachmentWidth, GLsizei attachmentHeight, bool withDepthStencil, const std::vector<Tex::InternalFormat>& attachmentFormats) {
   // Determine the texture size from the viewport
   GLint viewport[4];
   glGetIntegerv(GL_VIEWPORT, viewport);

   width = attachmentWidth <= 0 ? viewport[2] : attachmentWidth;
   height = attachmentHeight <= 0 ? viewport[3] : attachmentHeight;

   Context::current()->bindFramebuffer(fbo);

   depthStencilAttachment = nullptr;
   if (withDepthStencil) {
      Tex::Specification depthStencilSpecification = Tex::Specification::create2d();
      depthStencilSpecification.internalFormat = Tex::InternalFormat::kDepth24Stencil8;
      depthStencilSpecification.width = width;
      depthStencilSpecification.height = height;
      depthStencilSpecification.providedDataFormat = Tex::ProvidedDataFormat::kDepthStencil;
      depthStencilSpecification.providedDataType = Tex::ProvidedDataType::kUnsignedInt248;

      depthStencilAttachment = std::make_shared<Texture>(depthStencilSpecification);

      depthStencilAttachment->setParam(Tex::IntParam::kMinFilter, GL_NEAREST);
      depthStencilAttachment->setParam(Tex::IntParam::kMagFilter, GL_NEAREST);
      depthStencilAttachment->setParam(Tex::IntParam::kWrapS, GL_CLAMP_TO_EDGE);
      depthStencilAttachment->setParam(Tex::IntParam::kWrapT, GL_CLAMP_TO_EDGE);

      depthStencilAttachment->unbind();

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilAttachment->getId(), 0);
   }

   Tex::Specification colorSpecification = Tex::Specification::create2d();
   colorSpecification.width = width;
   colorSpecification.height = height;

   colorAttachments.resize(attachmentFormats.size());
   std::vector<GLenum> drawBuffers(attachmentFormats.size());
   for (GLsizei i = 0; i < attachmentFormats.size(); ++i) {
      colorSpecification.internalFormat = attachmentFormats[i];
      colorAttachments[i] = std::make_shared<Texture>(colorSpecification);
      drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

      colorAttachments[i]->setParam(Tex::IntParam::kMinFilter, GL_LINEAR);
      colorAttachments[i]->setParam(Tex::IntParam::kMagFilter, GL_LINEAR);
      colorAttachments[i]->setParam(Tex::IntParam::kWrapS, GL_CLAMP_TO_BORDER);
      colorAttachments[i]->setParam(Tex::IntParam::kWrapT, GL_CLAMP_TO_BORDER);

      colorAttachments[i]->unbind();

      glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[i], GL_TEXTURE_2D, colorAttachments[i]->getId(), 0);
   }
   glDrawBuffers(drawBuffers.size(), drawBuffers.data());

   ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
   bindDefaultFramebuffer();
}

void Framebuffer::setResolution(GLsizei attachmentWidth, GLsizei attachmentHeight) {
   std::vector<Tex::InternalFormat> internalFormats(colorAttachments.size());
   for (std::size_t i = 0; i < internalFormats.size(); ++i) {
      internalFormats[i] = colorAttachments[i]->getSpecification().internalFormat;
   }

   reset(attachmentWidth, attachmentHeight, depthStencilAttachment != nullptr, internalFormats);
}

void Framebuffer::bind() {
   Context::current()->bindFramebuffer(fbo);
   glViewport(0, 0, width, height);

   ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

} // namespace Shiny
