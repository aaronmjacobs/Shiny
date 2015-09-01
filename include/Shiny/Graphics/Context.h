#ifndef SHINY_CONTEXT_H
#define SHINY_CONTEXT_H

#include "Shiny/GLIncludes.h"

namespace Shiny {

class Context {
protected:
   static Context* currentContext;

   static void setCurrent(Context *context);

   static void onDestroy(Context *context);

   GLuint activeProgram;

public:
   static Context* current();

   Context();

   virtual ~Context();

   void makeCurrent();

   void useProgram(GLuint program);
};

} // namespace Shiny

#endif
