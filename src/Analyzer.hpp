#pragma once
#include <string>
#include <vector>
#include <map>
#include <ctime>

// ─────────────────────────────────────────────
//  Value object: metadata for a single file
// ─────────────────────────────────────────────
struct FileInfo {
    std::string path;       // full path
    std::string name;       // filename only
    std::string extension;  // e.g. ".cpp", ".txt", "" for no extension
    long long   size;       // bytes
    std::time_t modTime;    // last modification time (UNIX timestamp)
};

// ─────────────────────────────────────────────
//  Aggregate statistics for an entire directory
// ─────────────────────────────────────────────
struct DirectoryStats {
    std::vector<FileInfo>                          files;        // all scanned files
    std::map<std::string, std::vector<FileInfo>>   byExtension;  // grouped by extension
    long long                                       totalSize  = 0;
    int                                             totalFiles = 0;

    // Legacy C++-specific counters (kept for backward compat with analyze command)
    int       cppFiles    = 0;
    int       headerFiles = 0;
    long long totalLines  = 0;
};

// ─────────────────────────────────────────────
//  Analyzer — scans a directory tree
// ─────────────────────────────────────────────
class Analyzer {
public:
    // Scan directory recursively; populates all FileInfo + legacy fields
    static DirectoryStats analyze(const std::string& path);
};
