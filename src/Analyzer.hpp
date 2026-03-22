#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct DirectoryStats {
    int cppFiles = 0;
    int headerFiles = 0;
    long long totalLines = 0;
    long long totalSize = 0;
    std::unordered_map<std::string, int> extensionCounts;
};

class Analyzer {
public:
    static DirectoryStats analyze(const std::string& path);
};
