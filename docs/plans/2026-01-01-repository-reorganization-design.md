# Repository Reorganization Design

**Date:** 2026-01-01
**Status:** Approved

## Overview

Complete reorganization of the Parallel Minimum Interval Cover repository to establish a clean, maintainable directory structure with proper separation of source code, tests, benchmarks, tools, and build artifacts.

## Goals

1. **Clean up build artifacts**: Remove compiled executables and build outputs from git tracking
2. **Organize source files**: Establish logical directory structure (include/, tests/, benchmarks/, tools/)
3. **Improve maintainability**: Clear separation of concerns for easier navigation and development
4. **Prevent future pollution**: Enhanced .gitignore to prevent build artifacts from being committed

## Target Directory Structure

```
Parallel-Minimum-Interval-Cover/
├── README.md                          # Keep in root (standard)
├── CMakeLists.txt                     # Updated for new structure
├── .gitignore                         # Enhanced to exclude build artifacts
├── .gitmodules
├── include/
│   ├── interval_covering.h           # Core algorithm header
│   └── macro.h                        # Utility macros
├── src/                               # (Empty for now - only headers currently)
├── tests/
│   ├── test_interval_covering.cpp
│   ├── test_sample_duplicates.cpp
│   ├── test_simple_scan.cpp
│   ├── debug_comparison.cpp          # Debug utilities as tests
│   └── debug_scan_hang.cpp
├── benchmarks/
│   ├── benchmark_interval_covering.cpp
│   ├── benchmark_parallel_breakdown.cpp
│   └── benchmark_thread_scaling.cpp
├── results/                           # Benchmark results directory (gitignored)
│   └── *.csv                          # Benchmark output data files
├── tools/
│   ├── plot_breakdown.py
│   ├── plot_performance.py
│   ├── run_thread_scaling.sh
│   └── parallel_breakdown.csv        # Data files used by scripts
├── docs/
│   ├── plans/                         # Existing design docs
│   ├── WORK_SUMMARY.md               # Moved from root
│   ├── BUG_FIX_SUMMARY.md            # Moved from root
│   └── CHANGELOG.md                   # Moved from root
├── build/                             # Gitignored (build artifacts)
├── build_release/                     # Gitignored (build artifacts)
├── parlaylib/                         # Submodule (unchanged)
├── venv/                              # Gitignored (Python virtual env)
└── plots/                             # Gitignored (generated plot images)
```

## .gitignore Updates

Enhanced .gitignore to prevent build artifacts, executables, and generated files:

```gitignore
# Build directories and artifacts
build/
build_*/
*.o
*.a
*.so
*.dylib

# Executables
test_interval_covering
test_sample_duplicates
test_simple_scan
test_benchmark_flow
debug_scan_hang
debug_scan_hang_O3
debug_scan_hang_full
benchmark_interval_covering
benchmark_parallel_breakdown
benchmark_thread_scaling

# Generated results and outputs
results/
*.csv
plots/
*.png
*.pdf
*.svg

# Python
venv/
__pycache__/
*.pyc
*.pyo
*.egg-info/

# IDE and editor files
.vscode/
.idea/
*.swp
*.swo
*~

# Claude Code
.claude/

# OS files
.DS_Store
Thumbs.db
```

## CMakeLists.txt Changes

**Key modifications:**

1. Update include paths to use `include/` directory
2. Update source file paths for tests and benchmarks
3. Set output directory to `build/bin/`
4. All configuration in single main CMakeLists.txt (simpler for this project size)

```cmake
# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/parlaylib/include)

# Tests
add_executable(test_interval_covering tests/test_interval_covering.cpp)
add_executable(test_sample_duplicates tests/test_sample_duplicates.cpp)
add_executable(test_simple_scan tests/test_simple_scan.cpp)
add_executable(debug_comparison tests/debug_comparison.cpp)
add_executable(debug_scan_hang tests/debug_scan_hang.cpp)

# Benchmarks
add_executable(benchmark_interval_covering benchmarks/benchmark_interval_covering.cpp)
add_executable(benchmark_parallel_breakdown benchmarks/benchmark_parallel_breakdown.cpp)
add_executable(benchmark_thread_scaling benchmarks/benchmark_thread_scaling.cpp)

# Output binaries to build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
```

## Migration Steps

1. **Create new directory structure:**
   ```bash
   mkdir -p include src tests benchmarks tools docs results
   ```

2. **Move files to new locations:**
   ```bash
   # Headers
   git mv interval_covering.h macro.h include/

   # Tests
   git mv test_*.cpp debug_*.cpp tests/

   # Benchmarks
   git mv benchmark_*.cpp benchmarks/

   # Tools
   git mv *.py *.sh parallel_breakdown.csv tools/

   # Documentation
   git mv WORK_SUMMARY.md BUG_FIX_SUMMARY.md CHANGELOG.md docs/
   ```

3. **Update .gitignore** with enhanced patterns

4. **Clean up git-tracked build artifacts:**
   ```bash
   git rm -r --cached build_release/
   git rm --cached debug_scan_hang* test_benchmark_flow test_sample_duplicates test_simple_scan
   git rm -r --cached venv/ plots/
   ```

5. **Update CMakeLists.txt** with new paths

6. **Test build:**
   ```bash
   rm -rf build && mkdir build && cd build
   cmake ..
   make
   ./bin/test_interval_covering
   cd ..
   ```

7. **Commit changes:**
   ```bash
   git add .
   git commit -m "Reorganize repository structure"
   ```

## Validation Checklist

- [ ] All tests compile and run successfully
- [ ] All benchmarks compile successfully (runtime validation can be done later)
- [ ] Build artifacts are in build/ directory, not in root
- [ ] `git status` is clean (no untracked executables)
- [ ] README.md build instructions still work

## Notes

- Benchmark executables may have runtime issues to be addressed separately
- Files stay on disk but are properly gitignored to prevent future commits
- `results/` directory created for future benchmark output data
- All documentation centralized in `docs/` except README.md (standard practice)

## Decisions Made

1. **Directory structure**: Detailed organization with include/, src/, tests/, benchmarks/, tools/
2. **Debug files**: Moved to tests/ directory (treated as test utilities)
3. **Python directories**: venv/ and plots/ gitignored but kept locally
4. **Documentation**: All moved to docs/ except README.md
5. **Benchmark results**: Dedicated results/ directory (gitignored)
6. **Build configuration**: Single CMakeLists.txt (simpler for project size)
