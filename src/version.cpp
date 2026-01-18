// Git-Z version information
// Build date: 2026-01-19
// Compiler: MSVC 19.50.35722.0
// C++ standard: C++17

#include "Config.h"
#include <string>

namespace gitz {

std::string getVersionString() {
    return "Git-Z v" + std::string(Config::version()) + " (C++17)";
}

std::string getBuildInfo() {
#ifdef NDEBUG
    return "Release build";
#else
    return "Debug build";
#endif
}

} // namespace gitz
