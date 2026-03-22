#pragma once
#include <string>

class ProjectUtils {
public:
    // Returns true if the directory is a common build/source control folder
    // such as .git, build, cmake-build-debug, etc.
    static bool shouldIgnore(const std::string& path);
};
