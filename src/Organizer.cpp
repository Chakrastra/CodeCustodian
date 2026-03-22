#include "Organizer.hpp"
#include "ProjectUtils.hpp"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <direct.h> // For _mkdir
#include <string.h>
#include <cstdio> // for rename

void Organizer::organize(const std::string& path, bool dryRun) {
    std::string srcDir = path + "/src";
    std::string includeDir = path + "/include";

    if (!dryRun) {
        _mkdir(srcDir.c_str());
        _mkdir(includeDir.c_str());
    } else {
        std::cout << "[DRY RUN] Would create directories: " << srcDir << ", " << includeDir << "\n";
    }

    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "Failed to open directory: " << path << "\n";
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        std::string filename = entry->d_name;
        std::string fullPath = path + "/" + filename;
        
        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0) continue;

        if (S_ISREG(info.st_mode)) {
            size_t dotPos = filename.find_last_of(".");
            if (dotPos == std::string::npos) continue;

            std::string ext = filename.substr(dotPos);
            std::string destPath;

            if (ext == ".cpp" || ext == ".cc" || ext == ".cxx") {
                destPath = srcDir + "/" + filename;
            } else if (ext == ".h" || ext == ".hpp") {
                destPath = includeDir + "/" + filename;
            } else {
                continue;
            }

            if (dryRun) {
                std::cout << "[DRY RUN] Would move " << fullPath << " to " << destPath << "\n";
            } else {
                if (std::rename(fullPath.c_str(), destPath.c_str()) == 0) {
                    std::cout << "Moved: " << filename << "\n";
                } else {
                    perror("Error moving file");
                }
            }
        }
    }
    closedir(dir);
}
