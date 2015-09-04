#include "Shiny/ShinyAssert.h"

#include "Shiny/Input/ControllerMap.h"

#include <algorithm>
#include <cctype>

namespace Shiny {

namespace ControllerType {

namespace {

const char *kNameXbox360 = "Xbox 360";
const char *kNameXboxOne = "Xbox One";
const char *kNamePS3 = "PS3";
const char *kNamePS4 = "PS4";

const ControllerMap kNone = {
   nullptr,
   0.0f, 0.0f,
   { -1, false, false }, { -1, false, false }, { -1, false, false }, { -1, false, false },
   { -1, false, false }, { -1, false, false }, { -1, false, false }, { -1, false, false },
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

#ifdef __APPLE__

// TODO
const ControllerMap kXbox360 = kNone;

const ControllerMap kXboxOne = kNone;

const ControllerMap kPS3 = kNone;

const ControllerMap kPS4 = kNone;

#endif // __APPLE__

#ifdef __linux__

// TODO
const ControllerMap kXbox360 = kNone;

const ControllerMap kXboxOne = kNone;

const ControllerMap kPS3 = kNone;

const ControllerMap kPS4 = kNone;

#endif // __linux__

#ifdef _WIN32

// TODO
const ControllerMap kXbox360 = kNone;

const ControllerMap kXboxOne = kNone;

const ControllerMap kPS3 = kNone;

const ControllerMap kPS4 = kNone;

#endif // _WIN32

bool stringContainsIgnoreCase(const std::string &haystack, const std::string &needle) {
   return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                      [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); })
      != haystack.end();
}

} // namespace

const ControllerMap* none() {
   return &kNone;
}

#ifdef __APPLE__

// TODO
const ControllerMap* guess(const std::string &name, int numButtons, int numAxes) {
   ASSERT(numButtons >=0 && numAxes >= 0, "Invalid number of buttons (%d) and axes (%d)", numButtons, numAxes);
   return &kNone;
}

#endif // __APPLE__

#ifdef __linux__

// TODO
const ControllerMap* guess(const std::string &name, int numButtons, int numAxes) {
   ASSERT(numButtons >=0 && numAxes >= 0, "Invalid number of buttons (%d) and axes (%d)", numButtons, numAxes);
   return &kNone;
}

#endif // __linux__

#ifdef _WIN32

// TODO
const ControllerMap* guess(const std::string &name, int numButtons, int numAxes) {
   ASSERT(numButtons >=0 && numAxes >= 0, "Invalid number of buttons (%d) and axes (%d)", numButtons, numAxes);
   return &kNone;
}

#endif // _WIN32

} // namespace ControllerType

} // namespace Shiny
