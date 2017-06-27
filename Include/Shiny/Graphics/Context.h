#ifndef SHINY_CONTEXT_H
#define SHINY_CONTEXT_H

#include "Shiny/ShinyAssert.h"
#include "Shiny/Graphics/OpenGL.h"
#include "Shiny/Graphics/TextureInfo.h"
#include "Shiny/Graphics/Viewport.h"

#include <array>

namespace Shiny {

class Context {
public:
   static Context* current() {
      ASSERT(currentContext, "Current context is null");
      return currentContext;
   }

   Context();
   Context(const Context& other) = delete;
   Context(Context&& other) = delete;

   ~Context() {
      onDestroy(this);
   }

   Context& operator=(const Context& other) = delete;
   Context& operator=(Context&& other) = delete;

   void makeCurrent() {
      setCurrent(this);
   }

   void poll();

   void onFramebufferSizeChange(int width, int height);

   void useProgram(GLuint program);
   void bindVertexArray(GLuint vao);
   void bindTexture(Tex::Target target, GLuint texture);
   void bindFramebuffer(GLuint fbo);

   void onFramebufferDeleted(GLuint fbo);

   Viewport getViewport() const {
      return viewport;
   }

   GLint getMaxTextureSize() const {
      return maxTextureSize;
   }

private:
   static void setCurrent(Context *context);
   static void onDestroy(Context *context);

   static Context* currentContext;

   Viewport viewport;
   GLuint currentProgram;
   GLuint boundVAO;
   GLuint boundDrawFBO;
   GLuint boundReadFBO;
   std::array<GLuint, 10> boundTextures;

   GLint maxTextureSize;
};

} // namespace Shiny

#endif
