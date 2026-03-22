# CodeCustodian

CodeCustodian is a powerful, lightweight, cross-platform C++ command-line tool designed to help developers automatically organize, analyze, and automate maintenance tasks for their C++ codebases. 

## Features

1. **Analyze Project Statistics (`analyze`)**: Scans a directory to provide a quick summary of the codebase, counting C++ source files, header files, total lines of code, and total file size.
2. **Auto-Organize Files (`organize`)**: Instantly creates `src` and `include` folders and automatically sorts mixed C++ files into their respective directories. Use `--dry-run` to preview the changes safely.
3. **Find Unused Headers (`check-includes`)**: Finds any header files in your project that are never actually `#include`d in any `.cpp` source files, helping you prevent unused code bloat.
4. **Auto-Format Code (`format`)**: Runs `clang-format` on all `.cpp` and `.h` files to ensure consistent indentation and standard styling across the codebase. *(Requires LLVM/clang-format installed)*.

**Bonus: Smart Ignoring**
Built-in awareness ignores standard build and environment directories (`.git`, `build`, `node_modules`, `out`) ensuring the tool refrains from analyzing or moving compiled code.

## Compilation

Ensure you have a modern C++ compiler (like GCC, Clang, or MSVC) installed. Then, simply compile the project using following command:

```bash
g++ -std=c++17 src/*.cpp -o codecustodian
```

If you use CMake:
```bash
cmake -S . -B build
cmake --build build
```

## Usage

```bash
codecustodian <command> [options]
```

By default, the tool analyzes the current working directory. You can specify a custom path using the `--path` option.

### Examples

**Analyze a messy project directory:**
```bash
codecustodian analyze --path ./my_project
```

**Preview an automated organization (dry-run):**
```bash
codecustodian organize --path ./my_project --dry-run
```

**Find unused header files:**
```bash
codecustodian check-includes
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
