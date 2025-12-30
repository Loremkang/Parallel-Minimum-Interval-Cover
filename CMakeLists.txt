cmake_minimum_required(VERSION 3.10)
project(ParallelMinimumIntervalCover CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# Add ParlayLib
add_subdirectory(parlaylib EXCLUDE_FROM_ALL)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/parlaylib/include)

# Test executable
add_executable(test_interval_covering test_interval_covering.cpp)
target_link_libraries(test_interval_covering parlay)

# Optional: enable pthread
find_package(Threads REQUIRED)
target_link_libraries(test_interval_covering Threads::Threads)

# Add custom target for running tests
add_custom_target(run_tests
    COMMAND test_interval_covering
    DEPENDS test_interval_covering
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Benchmark executable
if(EXISTS "${CMAKE_SOURCE_DIR}/benchmark_interval_covering.cpp")
    add_executable(benchmark_interval_covering benchmark_interval_covering.cpp)
    target_link_libraries(benchmark_interval_covering parlay)
    target_link_libraries(benchmark_interval_covering Threads::Threads)

    add_custom_target(run_benchmark
        COMMAND benchmark_interval_covering
        DEPENDS benchmark_interval_covering
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Thread scaling benchmark executable
if(EXISTS "${CMAKE_SOURCE_DIR}/benchmark_thread_scaling.cpp")
    add_executable(benchmark_thread_scaling benchmark_thread_scaling.cpp)
    target_link_libraries(benchmark_thread_scaling parlay)
    target_link_libraries(benchmark_thread_scaling Threads::Threads)

    add_custom_target(run_thread_scaling
        COMMAND benchmark_thread_scaling
        DEPENDS benchmark_thread_scaling
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Bug demonstration executable
if(EXISTS "${CMAKE_SOURCE_DIR}/demo_parallel_bug.cpp")
    add_executable(demo_parallel_bug demo_parallel_bug.cpp)
    target_link_libraries(demo_parallel_bug parlay)
    target_link_libraries(demo_parallel_bug Threads::Threads)
endif()

# Debug comparison executable
if(EXISTS "${CMAKE_SOURCE_DIR}/debug_comparison.cpp")
    add_executable(debug_comparison debug_comparison.cpp)
    target_link_libraries(debug_comparison parlay)
    target_link_libraries(debug_comparison Threads::Threads)
endif()
