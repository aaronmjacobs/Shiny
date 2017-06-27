#include "Shiny/Graphics/Context.h"

#include <array>

namespace Shiny {

namespace {

std::size_t textureTargetIndex(Tex::Target textureTarget) {
   switch (textureTarget) {
   case Tex::Target::k1d:
      return 0;
   case Tex::Target::kBuffer:
      return 1;
   case Tex::Target::k2d:
      return 2;
   case Tex::Target::k1dArray:
      return 3;
   case Tex::Target::kRectangle:
      return 4;
   case Tex::Target::k2dMultisample:
      return 5;
   case Tex::Target::kCubeMap:
      return 6;
   case Tex::Target::k3d:
      return 7;
   case Tex::Target::k2dArray:
      return 8;
   case Tex::Target::k2dMultisampleArray:
      return 9;
   default:
      ASSERT(false, "Invalid texture target: %u", textureTarget);
      return 0;
   }
}

} // namespace

// static
Context* Context::currentContext = nullptr;

// static
void Context::setCurrent(Context *context) {
   currentContext = context;
}

// static
void Context::onDestroy(Context *context) {
   if (currentContext == context) {
      currentContext = nullptr;
   }
}

Context::Context()
   : currentProgram(0), boundVAO(0), boundDrawFBO(0), boundReadFBO(0), boundTextures{}, maxTextureSize(0) {
}

void Context::poll() {
   glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&currentProgram));
   glGetIntegerv(GL_VERTEX_ARRAY_BINDING, reinterpret_cast<GLint*>(&boundVAO));
   glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&boundDrawFBO));
   glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&boundReadFBO));

   glGetIntegerv(GL_VIEWPORT, viewport.data());
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
}

void Context::onFramebufferSizeChange(int width, int height) {
   viewport.width = width;
   viewport.height = height;

   if (boundDrawFBO == 0) {
      glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
   }
}

void Context::useProgram(GLuint program) {
   if (program != currentProgram) {
      glUseProgram(program);
      currentProgram = program;
   }
}

void Context::bindVertexArray(GLuint vao) {
   if (vao != boundVAO) {
      glBindVertexArray(vao);
      boundVAO = vao;
   }
}

void Context::bindTexture(Tex::Target target, GLuint texture) {
   std::size_t index = textureTargetIndex(target);

   if (boundTextures[index] != texture) {
      glBindTexture(static_cast<GLenum>(target), texture);
      boundTextures[index] = texture;
   }
}

void Context::bindFramebuffer(GLuint fbo) {
   if (fbo != boundDrawFBO) {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      boundDrawFBO = boundReadFBO = fbo;

      if (boundDrawFBO == 0) {
         glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
      }
   }
}

void Context::onFramebufferDeleted(GLuint fbo) {
   if (fbo == boundDrawFBO) {
      bindFramebuffer(0);
   }
}

} // namespace Shiny
