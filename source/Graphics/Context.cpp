#include "Context.h"

namespace Shiny {

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

// static
Context* Context::current() {
   return currentContext;
}

Context::Context()
   : activeProgram(0) {
}

Context::~Context() {
   onDestroy(this);
}

void Context::makeCurrent() {
   setCurrent(this);
}

void Context::useProgram(GLuint program) {
   if (program != activeProgram) {
      glUseProgram(program);
      activeProgram = program;
   }
}

} // namespace Shiny
