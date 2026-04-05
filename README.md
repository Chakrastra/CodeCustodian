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
============================================================
  FILE DISTRIBUTION REPORT
============================================================
  Scanned directory - total files: 26

  Extension     Files     Share
------------------------------------------------------------
  .cpp          7         26.9%
  .hpp          7         26.9%
  .txt          4         15.4%
  ...

============================================================
  STORAGE USAGE REPORT
============================================================
  Total storage: 1.85 MB

  Extension     Total Size    Files     Share
------------------------------------------------------------
  .exe          1.74 MB       3         94.2%
  .cpp          27.15 KB      7         1.4%
  ...

  Top 5 Largest Files:
------------------------------------------------------------
  codecustodian.exe                       1.44 MB
  ...

============================================================
  MODIFICATION TIME REPORT
============================================================

  Newest 5 Files:
------------------------------------------------------------
  Reporter.cpp                          2026-04-05 14:47
  ...

  Oldest 5 Files:
------------------------------------------------------------
  fs_compat.hpp                         2026-01-07 12:44
  ...
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
