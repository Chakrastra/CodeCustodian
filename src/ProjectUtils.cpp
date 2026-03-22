#include "ProjectUtils.hpp"
#include <algorithm>

bool ProjectUtils::shouldIgnore(const std::string& path) {
    std::string name;
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        name = path.substr(slashPos + 1);
    } else {
        name = path;
    }

    if (name.empty()) return false;
    
    // Convert to lowercase for comparison
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if (name == ".git" || name == ".svn" || name == ".hg" ||
        name == "build" || name == "out" || name == "target" ||
        name == "cmake-build-debug" || name == "cmake-build-release" ||
        name == "node_modules" || name == ".vs" || name == ".idea" ||
        name == "test_env") { // Also ignore test_env so we don't analyze our own test files unless specified directly
        return true;
    }
    return false;
}
