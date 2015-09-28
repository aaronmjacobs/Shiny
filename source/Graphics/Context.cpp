#include "Shiny/ShinyAssert.h"

#include "Shiny/Graphics/Context.h"

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
   ASSERT(currentContext, "Current context is null");
   return currentContext;
}

Context::Context()
   : currentProgram(0), boundVAO(0) {
}

Context::~Context() {
   onDestroy(this);
}

void Context::makeCurrent() {
   setCurrent(this);
}

void Context::poll() {
   glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&currentProgram));
   glGetIntegerv(GL_VERTEX_ARRAY_BINDING, reinterpret_cast<GLint*>(&boundVAO));
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

} // namespace Shiny
