#include "IncludeChecker.hpp"
#include "ProjectUtils.hpp"
#include <iostream>
#include <vector>
#include <unordered_set>
#include <regex>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void gatherFiles(const std::string& path, std::vector<std::string>& cppFiles, std::vector<std::string>& hppFiles) {
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
                gatherFiles(fullPath, cppFiles, hppFiles);
            }
        } else if (S_ISREG(info.st_mode)) {
            std::string filename = entry->d_name;
            size_t dotPos = filename.find_last_of(".");
            if (dotPos != std::string::npos) {
                std::string ext = filename.substr(dotPos);
                if (ext == ".cpp" || ext == ".cc" || ext == ".cxx") {
                    cppFiles.push_back(fullPath);
                } else if (ext == ".h" || ext == ".hpp") {
                    hppFiles.push_back(fullPath);
                }
            }
        }
    }
    closedir(dir);
}

void IncludeChecker::check(const std::string& path) {
    std::vector<std::string> cppFiles;
    std::vector<std::string> hppFiles;
    gatherFiles(path, cppFiles, hppFiles);

    std::unordered_set<std::string> includedHeaders;
    std::regex includeRegex(R"#(^\s*#include\s+["<]([^">]+)[">])#");

    for (const auto& cppFile : cppFiles) {
        std::ifstream file(cppFile);
        std::string line;
        while (std::getline(file, line)) {
            std::smatch match;
            if (std::regex_search(line, match, includeRegex)) {
                std::string headerName = match[1].str();
                size_t slashPos = headerName.find_last_of("/\\");
                if (slashPos != std::string::npos) {
                    headerName = headerName.substr(slashPos + 1);
                }
                includedHeaders.insert(headerName);
            }
        }
    }

    std::cout << "--- Unused Headers Report ---\n";
    int unusedCount = 0;
    for (const auto& hppFile : hppFiles) {
        std::string filename;
        size_t slashPos = hppFile.find_last_of("/\\");
        if (slashPos != std::string::npos) {
            filename = hppFile.substr(slashPos + 1);
        } else {
            filename = hppFile;
        }

        if (includedHeaders.find(filename) == includedHeaders.end()) {
            std::cout << "Potentially unused header: " << hppFile << "\n";
            unusedCount++;
        }
    }
    
    if (unusedCount == 0) {
        std::cout << "All headers appear to be used!\n";
    } else {
        std::cout << "Total potentially unused headers: " << unusedCount << "\n";
    }
}
