#include "Reporter.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>
#include <utility>
#include <ctime>

// ─── Private helpers ──────────────────────────────────────────────────────────

std::string Reporter::formatSize(long long bytes) {
    std::ostringstream oss;
    if (bytes < 1024LL) {
        oss << bytes << " B";
    } else if (bytes < 1024LL * 1024) {
        oss << std::fixed << std::setprecision(2) << bytes / 1024.0 << " KB";
    } else if (bytes < 1024LL * 1024 * 1024) {
        oss << std::fixed << std::setprecision(2) << bytes / (1024.0 * 1024) << " MB";
    } else {
        oss << std::fixed << std::setprecision(2) << bytes / (1024.0 * 1024 * 1024) << " GB";
    }
    return oss.str();
}

std::string Reporter::formatTime(std::time_t t) {
    char buf[32];
    struct tm* tm_info = localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", tm_info);
    return std::string(buf);
}

void Reporter::printSeparator(char c, int width) {
    std::cout << std::string(width, c) << "\n";
}

// ─── Distribution report ──────────────────────────────────────────────────────
void Reporter::printDistributionReport(const DirectoryStats& stats) {
    printSeparator('=');
    std::cout << "  FILE DISTRIBUTION REPORT\n";
    printSeparator('=');
    std::cout << "  Scanned directory - total files: " << stats.totalFiles << "\n\n";

    if (stats.byExtension.empty()) {
        std::cout << "  No files found.\n";
        return;
    }

    // Build sortable list of (extension, count) pairs (no structured bindings)
    std::vector<std::pair<std::string, int> > extCounts;
    extCounts.reserve(stats.byExtension.size());
    for (auto it = stats.byExtension.begin(); it != stats.byExtension.end(); ++it) {
        extCounts.push_back(std::make_pair(it->first, (int)it->second.size()));
    }
    std::sort(extCounts.begin(), extCounts.end(),
              [](const std::pair<std::string,int>& a, const std::pair<std::string,int>& b){
                  return a.second > b.second;
              });

    std::cout << std::left
              << std::setw(14) << "  Extension"
              << std::setw(10) << "Files"
              << "Share\n";
    printSeparator('-');

    for (size_t i = 0; i < extCounts.size(); ++i) {
        const std::string& ext   = extCounts[i].first;
        int                cnt   = extCounts[i].second;
        double             share = (stats.totalFiles > 0)
                                   ? 100.0 * cnt / stats.totalFiles : 0.0;
        std::string label = ext.empty() ? "(none)" : ext;
        std::cout << std::left
                  << std::setw(14) << ("  " + label)
                  << std::setw(10) << cnt
                  << std::fixed << std::setprecision(1) << share << "%\n";
    }
    printSeparator('-');
    std::cout << "\n";
}

// ─── Storage report ───────────────────────────────────────────────────────────
void Reporter::printStorageReport(const DirectoryStats& stats) {
    printSeparator('=');
    std::cout << "  STORAGE USAGE REPORT\n";
    printSeparator('=');
    std::cout << "  Total storage: " << formatSize(stats.totalSize) << "\n\n";

    if (stats.byExtension.empty()) {
        std::cout << "  No files found.\n";
        return;
    }

    // Build (extension, totalBytes) list
    std::vector<std::pair<std::string, long long> > extBytes;
    extBytes.reserve(stats.byExtension.size());
    for (auto it = stats.byExtension.begin(); it != stats.byExtension.end(); ++it) {
        long long total = 0;
        for (size_t j = 0; j < it->second.size(); ++j) total += it->second[j].size;
        extBytes.push_back(std::make_pair(it->first, total));
    }
    std::sort(extBytes.begin(), extBytes.end(),
              [](const std::pair<std::string,long long>& a,
                 const std::pair<std::string,long long>& b){
                  return a.second > b.second;
              });

    std::cout << std::left
              << std::setw(14) << "  Extension"
              << std::setw(14) << "Total Size"
              << std::setw(10) << "Files"
              << "Share\n";
    printSeparator('-');

    for (size_t i = 0; i < extBytes.size(); ++i) {
        const std::string& ext   = extBytes[i].first;
        long long          bytes = extBytes[i].second;
        double share = (stats.totalSize > 0)
                       ? 100.0 * bytes / stats.totalSize : 0.0;
        std::string label = ext.empty() ? "(none)" : ext;
        int count = (int)stats.byExtension.at(ext).size();
        std::cout << std::left
                  << std::setw(14) << ("  " + label)
                  << std::setw(14) << formatSize(bytes)
                  << std::setw(10) << count
                  << std::fixed << std::setprecision(1) << share << "%\n";
    }
    printSeparator('-');

    // Top-5 largest individual files
    std::vector<FileInfo> sorted = stats.files;
    std::sort(sorted.begin(), sorted.end(),
              [](const FileInfo& a, const FileInfo& b){ return a.size > b.size; });

    int topN = std::min(5, (int)sorted.size());
    std::cout << "\n  Top " << topN << " Largest Files:\n";
    printSeparator('-');
    for (int i = 0; i < topN; ++i) {
        std::cout << "  " << std::left << std::setw(40) << sorted[i].name
                  << formatSize(sorted[i].size) << "\n";
    }
    printSeparator('-');
    std::cout << "\n";
}

// ─── Time report ─────────────────────────────────────────────────────────────
void Reporter::printTimeReport(const DirectoryStats& stats) {
    printSeparator('=');
    std::cout << "  MODIFICATION TIME REPORT\n";
    printSeparator('=');

    if (stats.files.empty()) {
        std::cout << "  No files found.\n";
        return;
    }

    std::vector<FileInfo> sorted = stats.files;
    std::sort(sorted.begin(), sorted.end(),
              [](const FileInfo& a, const FileInfo& b){ return a.modTime > b.modTime; });

    int topN = std::min(5, (int)sorted.size());

    std::cout << "\n  Newest " << topN << " Files:\n";
    printSeparator('-');
    for (int i = 0; i < topN; ++i) {
        std::cout << "  " << std::left << std::setw(38) << sorted[i].name
                  << formatTime(sorted[i].modTime) << "\n";
    }

    std::cout << "\n  Oldest " << topN << " Files:\n";
    printSeparator('-');
    int sz = (int)sorted.size();
    for (int i = sz - 1; i >= sz - topN; --i) {
        std::cout << "  " << std::left << std::setw(38) << sorted[i].name
                  << formatTime(sorted[i].modTime) << "\n";
    }
    printSeparator('-');
    std::cout << "\n";
}

// ─── Full report ──────────────────────────────────────────────────────────────
void Reporter::printFullReport(const DirectoryStats& stats) {
    printDistributionReport(stats);
    printStorageReport(stats);
    printTimeReport(stats);
}
