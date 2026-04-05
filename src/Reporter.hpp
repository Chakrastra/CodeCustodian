#pragma once
#include "Analyzer.hpp"

// ─────────────────────────────────────────────────────────────────────────────
//  Reporter — generates formatted reports from DirectoryStats
//
//  Three report types:
//    printDistributionReport  — file count per extension, sorted by count desc
//    printStorageReport       — bytes per extension + top-5 largest files
//    printTimeReport          — newest and oldest 5 files by modification time
// ─────────────────────────────────────────────────────────────────────────────
class Reporter {
public:
    static void printDistributionReport(const DirectoryStats& stats);
    static void printStorageReport(const DirectoryStats& stats);
    static void printTimeReport(const DirectoryStats& stats);

    // Convenience: runs all three reports in sequence
    static void printFullReport(const DirectoryStats& stats);

private:
    static std::string formatSize(long long bytes);
    static std::string formatTime(std::time_t t);
    static void        printSeparator(char c = '-', int width = 60);
};
