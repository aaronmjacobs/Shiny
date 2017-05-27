#ifndef SHINY_CONTEXT_H
#define SHINY_CONTEXT_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class Context {
protected:
   static Context* currentContext;

   static void setCurrent(Context *context);

   static void onDestroy(Context *context);

   GLuint currentProgram { 0 };
   GLuint boundVAO { 0 };

public:
   static Context* current();

   Context() = default;

   Context(const Context &other) = delete;

   Context& operator=(const Context &other) = delete;

   ~Context();

   void makeCurrent();

   void poll();

   void useProgram(GLuint program);

   void bindVertexArray(GLuint vao);
};

} // namespace Shiny

#endif
