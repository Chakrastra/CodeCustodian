#include "Analyzer.hpp"
#include "ProjectUtils.hpp"
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <algorithm>

// ── Extract lowercase extension from a filename ──────────────────────────────
static std::string getExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == 0) return "";
    std::string ext = filename.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return ext;
}

// ── Count newlines in a file ──────────────────────────────────────────────────
static long long countLines(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return 0;
    long long n = 0;
    std::string line;
    while (std::getline(f, line)) ++n;
    return n;
}

// ── Recursive directory scan ──────────────────────────────────────────────────
static void analyzeRecursive(const std::string& path, DirectoryStats& stats) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        std::string name     = entry->d_name;
        std::string fullPath = path + "/" + name;

        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0) continue;

        if (S_ISDIR(info.st_mode)) {
            if (!ProjectUtils::shouldIgnore(fullPath))
                analyzeRecursive(fullPath, stats);
        } else if (S_ISREG(info.st_mode)) {
            FileInfo fi;
            fi.path      = fullPath;
            fi.name      = name;
            fi.extension = getExtension(name);
            fi.size      = static_cast<long long>(info.st_size);
            fi.modTime   = info.st_mtime;

            stats.files.push_back(fi);
            stats.byExtension[fi.extension].push_back(fi);
            stats.totalSize  += fi.size;
            stats.totalFiles++;

            // Legacy C++ line counters
            if (fi.extension == ".cpp" || fi.extension == ".cc" || fi.extension == ".cxx") {
                stats.cppFiles++;
                stats.totalLines += countLines(fullPath);
            } else if (fi.extension == ".h" || fi.extension == ".hpp") {
                stats.headerFiles++;
                stats.totalLines += countLines(fullPath);
            }
        }
    }
    closedir(dir);
}

// ── Public API ────────────────────────────────────────────────────────────────
DirectoryStats Analyzer::analyze(const std::string& path) {
    DirectoryStats stats;
    analyzeRecursive(path, stats);
    return stats;
}
