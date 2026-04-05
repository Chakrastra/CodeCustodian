# CodeCustodian

A command-line tool written in C++ to **analyze, organize, and report on files** based on extension, size, and modification time. Designed with object-oriented principles and the C++ STL for efficient processing.

---

## Features

| Command | Description |
|---|---|
| `analyze` | Scan all file types; show extension counts, total size, line count (C++ files) |
| `report` | Full 3-section report: file distribution, storage usage, modification times |
| `organize` | Sort C++ files into `src/` & `include/`, or group **all** files by extension |
| `check-includes` | Detect header files never `#include`d in any `.cpp` source |
| `format` | Auto-format C++ files using `clang-format` |

### Report Sections (`report` command)

- **File Distribution** — count and share (%) per extension, sorted by frequency
- **Storage Usage** — bytes per extension, total size, top-5 largest files
- **Modification Time** — newest and oldest 5 files with timestamps

### Smart Directory Ignoring

Automatically skips build/environment directories: `.git`, `build`, `out`, `node_modules`, `target`, `.vs`, `.idea`, and more.

---

## Project Structure

```
CodeCustodian/
├── src/
│   ├── main.cpp            # Entry point & CLI argument dispatch
│   ├── Analyzer.hpp/.cpp   # Scans directories; builds FileInfo & DirectoryStats
│   ├── Reporter.hpp/.cpp   # Formats distribution, storage & time reports
│   ├── Organizer.hpp/.cpp  # Moves files into sorted subdirectories
│   ├── IncludeChecker.hpp/.cpp  # Detects unused header files
│   ├── Formatter.hpp/.cpp  # Runs clang-format on C++ files
│   └── ProjectUtils.hpp/.cpp   # Shared ignore-list utility
├── CMakeLists.txt
├── LICENSE
└── README.md
```

---

## Build

### Using g++
```bash
g++ -std=c++17 src/*.cpp -o codecustodian
```

### Using CMake
```bash
cmake -S . -B build
cmake --build build
```

> Requires GCC/Clang with C++17 support.

---

## Usage

```
codecustodian <command> [options]

Options:
  --path <dir>   Target directory  (default: current directory)
  --dry-run      Preview changes without moving any files
  --by-ext       (organize) Group all file types into per-extension folders
  --help         Show help message
```

### Examples

**Analyze all files in a directory:**
```bash
codecustodian analyze --path ./my_project
```
```
Analysis Result for: ./my_project
------------------------------------------------------------
  Total Files:                26
  C++ Source Files:           7
  Header Files:               7
  Total Lines of Code:        830
  Total Size:                 32234 bytes

  Extensions found:
    .cpp          7 file(s)
    .hpp          7 file(s)
```

**Generate a full report:**
```bash
codecustodian report --path ./my_project
```
```
Generating report for: .

============================================================
  FILE DISTRIBUTION REPORT
============================================================
  Scanned directory - total files: 21

  Extension   Files     Share
------------------------------------------------------------
  .cpp        7         33.3%
  .hpp        6         28.6%
  (none)      2         9.5%
  .txt        2         9.5%
  .exe        1         4.8%
  .json       1         4.8%
  .md         1         4.8%
  .zip        1         4.8%
------------------------------------------------------------

============================================================
  STORAGE USAGE REPORT
============================================================
  Total storage: 1.55 MB

  Extension   Total Size    Files     Share
------------------------------------------------------------
  .exe        1.44 MB       1         93.0%
  .zip        72.94 KB      1         4.6%
  .cpp        26.65 KB      7         1.7%
  .md         5.18 KB       1         0.3%
  .hpp        4.03 KB       6         0.3%
  (none)      1.45 KB       2         0.1%
  .txt        337 B         2         0.0%
  .json       228 B         1         0.0%
------------------------------------------------------------

  Top 5 Largest Files:
------------------------------------------------------------
  codecustodian.exe                       1.44 MB
  CliFM.zip                               72.94 KB
  Reporter.cpp                            7.27 KB
  Organizer.cpp                           5.61 KB
  README.md                               5.18 KB
------------------------------------------------------------

============================================================
  MODIFICATION TIME REPORT
============================================================

  Newest 5 Files:
------------------------------------------------------------
  sample_report.txt                     2026-04-05 15:20
  README.md                             2026-04-05 15:05
  codecustodian.exe                     2026-04-05 14:54
  Organizer.cpp                         2026-04-05 14:53
  Analyzer.cpp                          2026-04-05 14:53

  Oldest 5 Files:
------------------------------------------------------------
  CliFM.zip                             2026-03-03 01:43
  IncludeChecker.hpp                    2026-03-22 02:07
  ProjectUtils.hpp                      2026-03-22 02:08
  ProjectUtils.cpp                      2026-03-22 02:08
  Formatter.hpp                         2026-03-22 02:08
------------------------------------------------------------
```

**Organize C++ files (classic mode):**
```bash
codecustodian organize --path ./my_project --dry-run
```

**Organize all files by extension (e.g. images/, docs/, cpp/):**
```bash
codecustodian organize --path ./my_project --by-ext --dry-run
```

**Find unused headers:**
```bash
codecustodian check-includes --path ./my_project
```

**Auto-format all C++ files:**
```bash
codecustodian format --path ./my_project
```
> Requires [LLVM clang-format](https://clang.llvm.org/docs/ClangFormat.html) installed and on `PATH`.

---

## Design

- **Object-Oriented** — each concern is its own class (`Analyzer`, `Reporter`, `Organizer`, `IncludeChecker`, `Formatter`)
- **STL-driven** — `std::vector`, `std::map`, `std::sort`, `std::unordered_set` used throughout for efficient data processing
- **`FileInfo` value-object** — captures `path`, `name`, `extension`, `size`, and `modTime` per file
- **`DirectoryStats` aggregate** — groups files by extension via `std::map<string, vector<FileInfo>>` for O(log n) lookups

---

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
