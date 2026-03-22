#include <iostream>
#include <string>
#include <sys/stat.h>
#include <direct.h>
#include "Analyzer.hpp"
#include "Organizer.hpp"
#include "IncludeChecker.hpp"
#include "Formatter.hpp"

// Simple helper to get current working directory
std::string getCurrentPath() {
    char buffer[1024];
    if (_getcwd(buffer, 1024)) {
        return std::string(buffer);
    }
    return ".";
}

void printUsage() {
    std::cout << "Usage: codecustodian [command] [options]\n"
              << "Commands:\n"
              << "  analyze         Analyze directory statistics\n"
              << "  organize        Organize files into folders\n"
              << "  check-includes  Find unused header files\n"
              << "  format          Format C++ files using clang-format\n"
              << "Options:\n"
              << "  --path <path>  Specify directory path (default: current)\n"
              << "  --dry-run      Simulate organization without moving files\n"
              << "  --help         Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }

    std::string command = argv[1];
    std::string targetPath = getCurrentPath();
    bool dryRun = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--path" && i + 1 < argc) {
            targetPath = argv[++i];
        } else if (arg == "--dry-run") {
            dryRun = true;
        } else if (arg == "--help") {
            printUsage();
            return 0;
        }
    }

    // Basic validation
    struct stat info;
    if (stat(targetPath.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
         std::cerr << "Error: Invalid directory path: " << targetPath << "\n";
         return 1;
    }

    if (command == "analyze") {
        auto stats = Analyzer::analyze(targetPath);
        std::cout << "Analysis Result for: " << targetPath << "\n";
        std::cout << "  C++ Source Files: " << stats.cppFiles << "\n";
        std::cout << "  Header Files: " << stats.headerFiles << "\n";
        std::cout << "  Total Lines of Code: " << stats.totalLines << "\n";
        std::cout << "  Total Size (bytes): " << stats.totalSize << "\n";
    } else if (command == "organize") {
        Organizer::organize(targetPath, dryRun);
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
