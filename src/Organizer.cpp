#include "Organizer.hpp"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <direct.h>   // _mkdir
#include <string.h>
#include <cstdio>     // rename
#include <algorithm>  // transform
#include <map>
#include <vector>

// ─── Helper: ensure a directory exists (create if absent) ────────────────────
static bool ensureDir(const std::string& path, bool dryRun) {
    struct stat info;
    if (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode)) {
        return true; // already exists
    }
    if (dryRun) {
        std::cout << "[DRY RUN] Would create directory: " << path << "\n";
        return true;
    }
    return _mkdir(path.c_str()) == 0;
}

// ─── Helper: map extension → friendly folder name ────────────────────────────
static std::string extToFolder(const std::string& ext) {
    if (ext.empty()) return "no_extension";
    // Strip the leading dot and lowercase
    std::string name = ext.substr(1);
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return name;
}

// ─── organize() ──────────────────────────────────────────────────────────────
void Organizer::organize(const std::string& path, bool dryRun, bool byExt) {

    // ── Classic mode (C++ files only) ──────────────────────────────────────
    if (!byExt) {
        std::string srcDir     = path + "/src";
        std::string includeDir = path + "/include";

        ensureDir(srcDir,     dryRun);
        ensureDir(includeDir, dryRun);

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
            if (stat(fullPath.c_str(), &info) != 0 || !S_ISREG(info.st_mode)) continue;

            size_t dotPos = filename.find_last_of('.');
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
                std::cout << "[DRY RUN] Would move: " << fullPath << "\n"
                          << "               -> "    << destPath  << "\n";
            } else {
                if (std::rename(fullPath.c_str(), destPath.c_str()) == 0) {
                    std::cout << "Moved: " << filename << "\n";
                } else {
                    perror("Error moving file");
                }
            }
        }
        closedir(dir);
        return;
    }

    // ── By-extension mode (all file types) ─────────────────────────────────
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "Failed to open directory: " << path << "\n";
        return;
    }

    // Collect all regular files first, then move them
    std::map<std::string, std::vector<std::string>> grouped; // ext -> [fullPath, ...]

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        std::string filename = entry->d_name;
        std::string fullPath = path + "/" + filename;

        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0 || !S_ISREG(info.st_mode)) continue;

        size_t dotPos = filename.find_last_of('.');
        std::string ext = (dotPos != std::string::npos) ? filename.substr(dotPos) : "";
        grouped[ext].push_back(fullPath);
    }
    closedir(dir);

    int movedCount = 0;
    for (auto it = grouped.begin(); it != grouped.end(); ++it) {
        const std::string& ext      = it->first;
        const std::vector<std::string>& fileList = it->second;
        std::string folderName = extToFolder(ext);
        std::string destDir    = path + "/" + folderName;

        ensureDir(destDir, dryRun);

        for (size_t k = 0; k < fileList.size(); ++k) {
            const std::string& src = fileList[k];
            // Extract filename
            size_t slashPos = src.find_last_of("/\\");
            std::string fname = (slashPos != std::string::npos) ? src.substr(slashPos + 1) : src;
            std::string dest  = destDir + "/" + fname;

            if (dryRun) {
                std::cout << "[DRY RUN] Would move: " << src  << "\n"
                          << "               -> "    << dest << "\n";
            } else {
                if (std::rename(src.c_str(), dest.c_str()) == 0) {
                    std::cout << "Moved: " << fname << " -> " << folderName << "/\n";
                    movedCount++;
                } else {
                    perror(("Error moving " + fname).c_str());
                }
            }
        }
    }

    if (!dryRun) {
        std::cout << "\nOrganized " << movedCount << " file(s) into extension folders.\n";
    }
}
