cmake_minimum_required(VERSION 3.10)
project(ParallelMinimumIntervalCover CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for better visibility of compilation flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable verbose output to show compilation commands
# Users can also use: cmake --build build --verbose
# or: make VERBOSE=1 / ninja -v
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Show full compilation commands")

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# Note: Do not set CMAKE_CXX_FLAGS_RELEASE or CMAKE_CXX_FLAGS_DEBUG globally
# because we use per-target compilation options for precise control

# Report build type
# Note: Individual targets override global build type settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Global Build Type: Debug")
    message(STATUS "  (Note: Test targets use Debug, Benchmarks use Release)")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Global Build Type: Release")
    message(STATUS "  (Note: Test targets use Debug, Benchmarks use Release)")
else()
    message(STATUS "Global Build Type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "  (Note: Test targets use Debug, Benchmarks use Release)")
endif()

# Add ParlayLib
add_subdirectory(parlaylib EXCLUDE_FROM_ALL)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/parlaylib/include)

# Output binaries to build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Test executables - Always use Debug mode
add_executable(test_interval_covering tests/test_interval_covering.cpp)
target_link_libraries(test_interval_covering parlay)
target_compile_options(test_interval_covering PRIVATE -g -O0)
target_compile_definitions(test_interval_covering PRIVATE DEBUG)
message(STATUS "test_interval_covering: Debug mode (-g -O0 -DDEBUG)")

add_executable(test_sample_duplicates tests/test_sample_duplicates.cpp)
target_link_libraries(test_sample_duplicates parlay)
target_compile_options(test_sample_duplicates PRIVATE -g -O0)
target_compile_definitions(test_sample_duplicates PRIVATE DEBUG)
message(STATUS "test_sample_duplicates: Debug mode (-g -O0 -DDEBUG)")

add_executable(test_simple_scan tests/test_simple_scan.cpp)
target_link_libraries(test_simple_scan parlay)
target_compile_options(test_simple_scan PRIVATE -g -O0)
target_compile_definitions(test_simple_scan PRIVATE DEBUG)
message(STATUS "test_simple_scan: Debug mode (-g -O0 -DDEBUG)")

add_executable(debug_comparison tests/debug_comparison.cpp)
target_link_libraries(debug_comparison parlay)
target_compile_options(debug_comparison PRIVATE -g -O0)
target_compile_definitions(debug_comparison PRIVATE DEBUG)
message(STATUS "debug_comparison: Debug mode (-g -O0 -DDEBUG)")

add_executable(debug_scan_hang tests/debug_scan_hang.cpp)
target_link_libraries(debug_scan_hang parlay)
target_compile_options(debug_scan_hang PRIVATE -g -O0)
target_compile_definitions(debug_scan_hang PRIVATE DEBUG)
message(STATUS "debug_scan_hang: Debug mode (-g -O0 -DDEBUG)")

add_executable(test_generate_intervals tests/test_generate_intervals.cpp)
target_link_libraries(test_generate_intervals parlay)
target_compile_options(test_generate_intervals PRIVATE -g -O0)
target_compile_definitions(test_generate_intervals PRIVATE DEBUG)
message(STATUS "test_generate_intervals: Debug mode (-g -O0 -DDEBUG)")

# Optional: enable pthread
find_package(Threads REQUIRED)
target_link_libraries(test_interval_covering Threads::Threads)
target_link_libraries(test_sample_duplicates Threads::Threads)
target_link_libraries(test_simple_scan Threads::Threads)
target_link_libraries(debug_comparison Threads::Threads)
target_link_libraries(debug_scan_hang Threads::Threads)
target_link_libraries(test_generate_intervals Threads::Threads)

# Add custom target for running tests
add_custom_target(run_tests
    COMMAND test_interval_covering
    DEPENDS test_interval_covering
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Benchmark executables - Always use Release mode
if(EXISTS "${CMAKE_SOURCE_DIR}/benchmarks/benchmark_interval_covering.cpp")
    add_executable(benchmark_interval_covering benchmarks/benchmark_interval_covering.cpp)
    target_link_libraries(benchmark_interval_covering parlay)
    target_link_libraries(benchmark_interval_covering Threads::Threads)
    target_compile_options(benchmark_interval_covering PRIVATE -O3 -march=native)
    target_compile_definitions(benchmark_interval_covering PRIVATE NDEBUG)
    message(STATUS "benchmark_interval_covering: Release mode (-O3 -march=native -DNDEBUG)")

    add_custom_target(run_benchmark
        COMMAND benchmark_interval_covering
        DEPENDS benchmark_interval_covering
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/benchmarks/benchmark_thread_scaling.cpp")
    add_executable(benchmark_thread_scaling benchmarks/benchmark_thread_scaling.cpp)
    target_link_libraries(benchmark_thread_scaling parlay)
    target_link_libraries(benchmark_thread_scaling Threads::Threads)
    target_compile_options(benchmark_thread_scaling PRIVATE -O3 -march=native)
    target_compile_definitions(benchmark_thread_scaling PRIVATE NDEBUG)
    message(STATUS "benchmark_thread_scaling: Release mode (-O3 -march=native -DNDEBUG)")

    add_custom_target(run_thread_scaling
        COMMAND benchmark_thread_scaling
        DEPENDS benchmark_thread_scaling
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/benchmarks/benchmark_parallel_breakdown.cpp")
    add_executable(benchmark_parallel_breakdown benchmarks/benchmark_parallel_breakdown.cpp)
    target_link_libraries(benchmark_parallel_breakdown parlay)
    target_link_libraries(benchmark_parallel_breakdown Threads::Threads)
    target_compile_options(benchmark_parallel_breakdown PRIVATE -O3 -march=native)
    target_compile_definitions(benchmark_parallel_breakdown PRIVATE NDEBUG)
    message(STATUS "benchmark_parallel_breakdown: Release mode (-O3 -march=native -DNDEBUG)")

    add_custom_target(run_breakdown
        COMMAND benchmark_parallel_breakdown
        DEPENDS benchmark_parallel_breakdown
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Print compilation summary
message(STATUS "")
message(STATUS "=================================================================")
message(STATUS "                   COMPILATION OPTIONS SUMMARY")
message(STATUS "=================================================================")
message(STATUS "")
if(CMAKE_BUILD_TYPE)
    message(STATUS "Global CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    message(STATUS "  WARNING: CMAKE_BUILD_TYPE affects all targets!")
    message(STATUS "  For clean per-target flags, build without CMAKE_BUILD_TYPE:")
    message(STATUS "    cmake .. (without -DCMAKE_BUILD_TYPE=...)")
else()
    message(STATUS "Global CMAKE_BUILD_TYPE: Not set (recommended)")
    message(STATUS "  Each target uses its own optimized compilation flags")
endif()
message(STATUS "")
message(STATUS "Test Executables (Always Debug mode):")
message(STATUS "  - Compilation flags: -g -O0 -DDEBUG")
message(STATUS "  - Targets:")
message(STATUS "    • test_interval_covering")
message(STATUS "    • test_sample_duplicates")
message(STATUS "    • test_simple_scan")
message(STATUS "    • debug_comparison")
message(STATUS "    • debug_scan_hang")
message(STATUS "    • test_generate_intervals")
message(STATUS "")
message(STATUS "Benchmark Executables (Always Release mode):")
message(STATUS "  - Compilation flags: -O3 -march=native -DNDEBUG")
message(STATUS "  - Targets:")
message(STATUS "    • benchmark_interval_covering")
message(STATUS "    • benchmark_thread_scaling")
message(STATUS "    • benchmark_parallel_breakdown")
message(STATUS "")
message(STATUS "To see detailed compilation commands during build:")
message(STATUS "  • ninja -v")
message(STATUS "  • make VERBOSE=1")
message(STATUS "  • Or check: ${CMAKE_BINARY_DIR}/compile_commands.json")
message(STATUS "=================================================================")
message(STATUS "")

