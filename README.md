# Parallel Minimum Interval Cover

A high-performance parallel implementation of the Minimum Interval Cover problem using the ParlayLib parallel primitives library.

## Problem Description

The Minimum Interval Cover problem finds the minimum set of intervals needed to cover the union of all intervals. Given a set of intervals with monotonically increasing left and right endpoints, the algorithm efficiently identifies which intervals are necessary in the optimal covering.

## Algorithm Overview

The parallel algorithm consists of three main phases:

1. **Find Furthest Intersecting Intervals**: For each interval, find the furthest (rightmost) interval that intersects with it. This uses binary search and exploits the monotonicity property of the input.

2. **Build Euler Tour Tree**: Treat each interval as a node with its furthest intersecting interval as parent, forming a tree structure. Construct an Euler tour of this tree using a linked list representation.

3. **Scan and Mark Valid Intervals**: Perform a parallel scan on the Euler tour to propagate validity flags. Intervals on the path from the starting point to the root are marked as valid - these form the minimum interval cover.

## Features

- **Parallel and Serial Implementations**: Both parallel and serial versions for comparison and validation
- **Debug Mode**: Extensive validation by comparing parallel and serial results
- **Performance Optimizations**: Uses ParlayLib's efficient parallel primitives
- **Comprehensive Testing**: Unit tests covering various edge cases
- **Performance Benchmarking**: Tools to measure single-threaded and multi-threaded performance

## Requirements

- C++17 or later
- CMake 3.10 or later
- [ParlayLib](https://github.com/cmuparlay/parlaylib) (included as submodule)

## Building

```bash
# Clone the repository
git clone <repository-url>
cd Parallel-Minimum-Interval-Cover

# Initialize submodules
git submodule update --init --recursive

# Build
mkdir build && cd build
cmake ..
make

# Run tests
./bin/test_interval_covering

# Run benchmarks
./bin/benchmark_interval_covering
```

## Building with Debug Mode

To enable extensive validation:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON ..
make
```

## Usage Example

```cpp
#include "interval_covering.h"

// Define your interval data structure
std::vector<std::pair<int, int>> intervals = {
    {0, 5}, {1, 8}, {3, 10}, {7, 15}, {12, 20}
};

// Create lambda functions for accessing L and R
auto getL = [&](size_t i) { return intervals[i].first; };
auto getR = [&](size_t i) { return intervals[i].second; };

// Create solver instance
IntervalCovering<decltype(getL), decltype(getR)> solver;
solver.n = intervals.size();
solver.L = getL;
solver.R = getR;

// Run the algorithm
solver.Run();

// Access results
for (size_t i = 0; i < solver.n; i++) {
    if (solver.valid[i]) {
        std::cout << "Interval " << i << " is in minimum cover\n";
    }
}
```

## Performance

The algorithm achieves significant speedup on multi-core systems. See benchmark results for detailed performance analysis across different input sizes and thread counts.

## License

MIT License

## References

- [ParlayLib](https://github.com/cmuparlay/parlaylib)
