#include "Analyzer.hpp"
#include "ProjectUtils.hpp"
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void analyzeRecursive(const std::string& path, DirectoryStats& stats) {
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
                analyzeRecursive(fullPath, stats);
            }
        } else if (S_ISREG(info.st_mode)) {
            std::string filename = entry->d_name;
            size_t dotPos = filename.find_last_of(".");
            if (dotPos != std::string::npos) {
                std::string ext = filename.substr(dotPos);
                
                if (ext == ".cpp" || ext == ".cc" || ext == ".cxx") {
                    stats.cppFiles++;
                } else if (ext == ".h" || ext == ".hpp") {
                    stats.headerFiles++;
                } else {
                    continue;
                }

                stats.extensionCounts[ext]++;
                stats.totalSize += info.st_size;

                std::ifstream file(fullPath);
                if (file.is_open()) {
                    std::string line;
                    while (std::getline(file, line)) {
                        stats.totalLines++;
                    }
                }
            }
        }
    }
    closedir(dir);
}

DirectoryStats Analyzer::analyze(const std::string& path) {
    DirectoryStats stats;
    analyzeRecursive(path, stats);
    return stats;
}
