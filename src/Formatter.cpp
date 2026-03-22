#include "Formatter.hpp"
#include "ProjectUtils.hpp"
#include <iostream>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <cstdlib>

void gatherFormatFiles(const std::string& path, std::vector<std::string>& files) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        std::string fullPath = path + "/" + entry->d_name;
        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0) continue;

        if (S_ISDIR(info.st_mode)) {
            if (!ProjectUtils::shouldIgnore(fullPath)) {
                gatherFormatFiles(fullPath, files);
            }
        } else if (S_ISREG(info.st_mode)) {
            std::string filename = entry->d_name;
            size_t dotPos = filename.find_last_of(".");
            if (dotPos != std::string::npos) {
                std::string ext = filename.substr(dotPos);
                if (ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".h" || ext == ".hpp") {
                    files.push_back(fullPath);
                }
            }
        }
    }
    closedir(dir);
}

void Formatter::format(const std::string& path) {
#ifdef _WIN32
    if (std::system("clang-format --version > nul 2>&1") != 0) {
#else
    if (std::system("clang-format --version > /dev/null 2>&1") != 0) {
#endif
        std::cerr << "clang-format is not installed or not in PATH.\n";
        return;
    }

    std::vector<std::string> files;
    gatherFormatFiles(path, files);

    int count = 0;
    for (const auto& file : files) {
        std::string command = "clang-format -i \"" + file + "\"";
        if (std::system(command.c_str()) == 0) {
            count++;
        } else {
            std::cerr << "Failed to format: " << file << "\n";
        }
    }

    std::cout << "Formatted " << count << " files.\n";
}
