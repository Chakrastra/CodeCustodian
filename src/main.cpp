#include <iostream>
#include <string>
#include <iomanip>
#include <sys/stat.h>
#include <direct.h>
#include "Analyzer.hpp"
#include "Reporter.hpp"
#include "Organizer.hpp"
#include "IncludeChecker.hpp"
#include "Formatter.hpp"

// ─── Helpers ──────────────────────────────────────────────────────────────────

static std::string getCurrentPath() {
    char buffer[1024];
    if (_getcwd(buffer, 1024)) return std::string(buffer);
    return ".";
}

static void printUsage() {
    std::cout
        << "\nUsage: codecustodian <command> [options]\n\n"
        << "Commands:\n"
        << "  analyze         Analyze directory: extension counts, size, newest files\n"
        << "  report          Full report: distribution, storage usage, modification times\n"
        << "  organize        Organize files (classic: .cpp->src/, .h->include/)\n"
        << "  check-includes  Find unused header files\n"
        << "  format          Format C++ files using clang-format\n\n"
        << "Options:\n"
        << "  --path <dir>   Target directory (default: current directory)\n"
        << "  --dry-run      Preview changes without moving any files\n"
        << "  --by-ext       (organize) Group all files by extension into sub-folders\n"
        << "  --help         Show this help message\n\n"
        << "Examples:\n"
        << "  codecustodian analyze --path ./myproject\n"
        << "  codecustodian report  --path ./myproject\n"
        << "  codecustodian organize --path ./myproject --by-ext --dry-run\n\n";
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }

    std::string command    = argv[1];
    std::string targetPath = getCurrentPath();
    bool        dryRun     = false;
    bool        byExt      = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--path" && i + 1 < argc) {
            targetPath = argv[++i];
        } else if (arg == "--dry-run") {
            dryRun = true;
        } else if (arg == "--by-ext") {
            byExt = true;
        } else if (arg == "--help") {
            printUsage();
            return 0;
        }
    }

    // Validate path
    struct stat info;
    if (stat(targetPath.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        std::cerr << "Error: Invalid directory path: " << targetPath << "\n";
        return 1;
    }

    // ── Dispatch ──────────────────────────────────────────────────────────────

    if (command == "analyze") {
        // Enriched analyze: all extensions + C++ line count legacy output
        auto stats = Analyzer::analyze(targetPath);
        std::cout << "\nAnalysis Result for: " << targetPath << "\n";
        std::cout << std::string(60, '-') << "\n";
        std::cout << std::left << std::setw(30) << "  Total Files:"
                  << stats.totalFiles << "\n";
        std::cout << std::left << std::setw(30) << "  C++ Source Files:"
                  << stats.cppFiles << "\n";
        std::cout << std::left << std::setw(30) << "  Header Files:"
                  << stats.headerFiles << "\n";
        std::cout << std::left << std::setw(30) << "  Total Lines of Code:"
                  << stats.totalLines << "\n";
        std::cout << std::left << std::setw(30) << "  Total Size:"
                  << stats.totalSize << " bytes\n";

        // Extension breakdown
        if (!stats.byExtension.empty()) {
            std::cout << "\n  Extensions found:\n";
            for (const auto& kv : stats.byExtension) {
                std::string label = kv.first.empty() ? "(no extension)" : kv.first;
                std::cout << "    " << std::left << std::setw(14) << label
                          << kv.second.size() << " file(s)\n";
            }
        }
        std::cout << std::string(60, '-') << "\n\n";

    } else if (command == "report") {
        // Full formatted report
        auto stats = Analyzer::analyze(targetPath);
        std::cout << "\nGenerating report for: " << targetPath << "\n\n";
        Reporter::printFullReport(stats);

    } else if (command == "organize") {
        Organizer::organize(targetPath, dryRun, byExt);

    } else if (command == "check-includes") {
        IncludeChecker::check(targetPath);

    } else if (command == "format") {
        Formatter::format(targetPath);

    } else {
        std::cerr << "Unknown command: " << command << "\n";
        printUsage();
        return 1;
    }

    return 0;
}
