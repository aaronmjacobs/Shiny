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

const ControllerMap kXbox360 = {
   kNameXbox360,
   0.7f, 0.2f,
   { 0, true, false }, { 1, true, false }, { 2, true, false }, { 3, true, false },
   { 4, false, false }, { 5, false, false }, { -1, false, false }, { -1, false, false },
   2, 3, 0, 1, 13, 12, 14, 11, -1, -1, 8, 9, 6, 7, 5, 4
};

const ControllerMap kXboxOne = {
   kNameXboxOne,
   0.7f, 0.2f,
   { 0, true, false }, { 1, true, false }, { 2, true, false }, { 3, true, false },
   { 4, false, false }, { 5, false, false }, { -1, false, false }, { -1, false, false },
   2, 3, 0, 1, 13, 12, 14, 11, -1, -1, 8, 9, 6, 7, 5, 4
};

const ControllerMap kPS3 = {
   kNamePS3,
   0.7f, 0.1f,
   { 0, true, false }, { 1, true, false }, { 2, true, false }, { 3, true, false },
   { -1, false, false }, { -1, false, false }, { -1, false, false }, { -1, false, false },
   7, 5, 4, 6, 15, 13, 12, 14, 8, 9, 10, 11, 1, 2, 0, 3
};

const ControllerMap kPS4 = {
   kNamePS4,
   0.7f, 0.1f,
   { 0, true, false }, { 1, true, false }, { 2, true, false }, { 3, true, false },
   { 4, false, false }, { 5, false, false }, { -1, false, false }, { -1, false, false },
   17, 15, 14, 16, 0, 2, 3, 1, 6, 7, 4, 5, 10, 11, 8, 9
};

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
