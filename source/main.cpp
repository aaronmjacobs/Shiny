#include "Constants.h"
#include "Engine.h"
#include "GLIncludes.h"
#include "Log.h"
#include "Shiny.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cstdlib>

namespace {

class ExampleEngine : public Shiny::Engine {
private:
   float runningTime;

public:
   ExampleEngine() : runningTime(0.0f) {}

   virtual ~ExampleEngine() {}

   virtual void tick(const float dt) { runningTime += dt; }

   virtual void render();
};

void ExampleEngine::render() {
   float offset = glm::pi<float>() * 2.0f / 3.0f;
   float red = (glm::sin(runningTime) + 1.0f) / 2.0f;
   float green = (glm::sin(runningTime + offset) + 1.0f) / 2.0f;
   float blue = (glm::sin(runningTime + offset * 2.0f) + 1.0f) / 2.0f;

   glClearColor(red, green, blue, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace

#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif
   LOG_INFO(PROJECT_NAME << " " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_MICRO << " (" <<
            VERSION_BUILD << ")");

   Shiny::Result initResult = Shiny::init();
   if (initResult != Shiny::Result::kOK) {
      LOG_ERROR(Shiny::errorString(initResult));
      return EXIT_FAILURE;
   }

   int retVal = EXIT_SUCCESS;
   {
      ExampleEngine engine;

      Shiny::Result startResult = engine.startUp(1280, 720, "Shiny");
      if (startResult == Shiny::Result::kOK) {
         engine.run();
      } else {
         LOG_ERROR(Shiny::errorString(startResult));
         retVal = EXIT_FAILURE;
      }
   }

   Shiny::terminate();

   return retVal;
}
