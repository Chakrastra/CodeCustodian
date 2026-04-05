#pragma once
#include <string>

class Organizer {
public:
    // Classic mode: move .cpp → src/, .h/.hpp → include/
    // by-ext mode:  move every file → <extension>/ folder
    static void organize(const std::string& path, bool dryRun, bool byExt = false);
};
