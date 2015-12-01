#ifndef SHINY_CONTEXT_H
#define SHINY_CONTEXT_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

class Context {
protected:
   static Context* currentContext;

   static void setCurrent(Context *context);

   static void onDestroy(Context *context);

   GLuint currentProgram;
   GLuint boundVAO;

public:
   static Context* current();

   Context();

   virtual ~Context();

   void makeCurrent();

   void poll();

   void useProgram(GLuint program);

   void bindVertexArray(GLuint vao);
};

} // namespace Shiny

#endif
