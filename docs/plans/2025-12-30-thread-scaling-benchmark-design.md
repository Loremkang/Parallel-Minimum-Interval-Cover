# Thread Scaling Performance Benchmark Design

**Date:** 2025-12-30
**Status:** Approved
**Goal:** Benchmark and visualize single-threaded vs multi-threaded performance of the parallel minimum interval covering algorithm

## Background

The parallel interval covering implementation uses ParlayLib and has both serial and parallel algorithms:
- **Serial algorithm (KernelSerial)**: Simple sequential implementation with minimal overhead
- **Parallel algorithm (Run/KernelParallel)**: Uses Euler tour and parallel primitives, has overhead but scales with threads

Recent fixes to the parallel version (BuildLinkList) may have resolved circular linked list issues.

## Design Goals

1. Verify parallel implementation works correctly
2. Measure overhead of parallel algorithm structure (parallel-1-thread vs serial)
3. Measure parallel scaling efficiency (2, 4, 8, 16+ threads)
4. Generate comprehensive performance visualizations
5. Identify optimal thread counts for different input sizes

## Benchmark Design

### Test Configurations

**1. Serial Algorithm Baseline:**
- Uses `KernelSerial()` directly
- Single-threaded, minimal overhead
- Expected to be fastest for single-core
- Represents best sequential performance

**2. Parallel Algorithm with 1 Thread:**
- Uses `Run()/KernelParallel()` with `parlay::set_num_workers(1)`
- Has overhead from parallel algorithm structure
- Expected to be SLOWER than serial
- Shows cost of parallelization

**3. Parallel Algorithm with Multiple Threads:**
- Same parallel implementation
- Test with: 2, 4, 8, 16, 32 threads (up to hardware max)
- Should show speedup as threads increase
- Goal: find crossover point where parallel beats serial

### Input Sizes

- Small: 10K, 100K intervals (quick validation)
- Medium: 1M, 10M intervals (realistic workloads)
- Large: 50M, 100M intervals (strong scaling analysis)
- Use same random seed for reproducibility

### Metrics Collected

- Execution time (wall clock, milliseconds)
- Number of intervals selected (correctness verification)
- Throughput (millions of intervals/second)
- Overhead ratio: (Parallel-1-thread time) / (Serial time)
- Speedup: (Parallel-1-thread time) / (Parallel-N-thread time)
- Efficiency: (Speedup / N) × 100%

### Output Format

CSV file: `thread_scaling_results.csv`
```
algorithm,n,threads,time_ms,selected,throughput_M_per_sec
serial,1000000,1,5.23,521045,191.2
parallel,1000000,1,7.89,521045,126.7
parallel,1000000,2,4.12,521045,242.7
parallel,1000000,4,2.34,521045,427.4
...
```

## Visualization Design

### Graph 1: Execution Time vs Input Size
- **X-axis:** Input size (log scale: 10K to 100M)
- **Y-axis:** Time in milliseconds (log scale)
- **Lines:** Serial, Parallel-1, Parallel-2, Parallel-4, Parallel-8, etc.
- **Purpose:** Shows how each approach scales with data size
- **Insight:** Identifies when parallel overhead is justified

### Graph 2: Speedup vs Thread Count (Strong Scaling)
- **X-axis:** Number of threads (1, 2, 4, 8, 16, 32)
- **Y-axis:** Speedup relative to serial
- **Baseline:** Horizontal dashed line at y=1.0 (serial performance)
- **Reference:** Ideal linear speedup line (y=x)
- **Lines:** Multiple curves for different input sizes
- **Purpose:** Classic parallel performance metric
- **Insight:** Shows when and how much parallel beats serial

### Graph 3: Throughput vs Thread Count
- **X-axis:** Number of threads
- **Y-axis:** Throughput (millions of intervals/second)
- **Baseline:** Horizontal dashed line showing serial throughput
- **Lines:** Multiple curves for different input sizes
- **Purpose:** Practical capacity planning metric
- **Insight:** Maximum processing rate achievable

### Graph 4: Parallel Efficiency vs Thread Count
- **X-axis:** Number of threads
- **Y-axis:** Efficiency percentage (0-100%)
- **Formula:** Efficiency = (Speedup / Threads) × 100%
  - Speedup calculated relative to parallel-1-thread
  - 100% = ideal linear scaling
- **Purpose:** Shows parallel scaling quality
- **Insight:** Identifies diminishing returns and optimal thread count

## Implementation Plan

### Phase 1: Verification
1. Read current `interval_covering.h` to check parallel fixes
2. Build and run existing tests with parallel enabled
3. Run existing benchmark to verify parallel works
4. If issues found, use systematic debugging

### Phase 2: Benchmark Implementation
1. Create `benchmark_thread_scaling.cpp`:
   - Test serial algorithm baseline
   - Test parallel with configurable thread counts
   - Multiple runs per configuration (take median)
   - Output detailed CSV results

2. Thread control:
   - Use `parlay::set_num_workers(num_threads)`
   - Verify with `parlay::num_workers()`
   - Reset between tests for isolation

3. Stability measures:
   - Run each configuration 3 times
   - Take median to reduce noise
   - Detect and handle outliers

### Phase 3: Visualization Scripts
1. `plot_performance.py`:
   - Read CSV data
   - Generate all 4 graphs using matplotlib
   - Save as both PNG and PDF
   - Clean, publication-quality styling

2. `plot_performance.gnuplot` (fallback):
   - Same graphs if Python unavailable
   - Generate PNG output

### Phase 4: Execution & Validation
1. Build: `cmake .. && make benchmark_thread_scaling`
2. Run: `./benchmark_thread_scaling`
3. Plot: `python3 plot_performance.py`
4. Verify:
   - Serial faster than parallel-1-thread ✓
   - Parallel shows speedup with threads ✓
   - All graphs generated correctly ✓
   - Results make sense ✓

### Phase 5: Documentation
1. Update `WORK_SUMMARY.md` with results
2. Add performance analysis summary
3. Commit changes with descriptive message

## Expected Results

### Overhead Analysis
- Parallel-1-thread should be ~1.2-1.5× slower than serial
- Overhead comes from: Euler tour construction, linked list building, parallel primitives

### Scaling Analysis
- Small inputs (10K-100K): Parallel may not beat serial even with many threads
- Medium inputs (1M-10M): Parallel should beat serial with 4-8 threads
- Large inputs (50M+): Parallel should show strong scaling up to 16-32 threads

### Break-even Point
- Expected crossover: 4-8 threads for 10M intervals
- Larger inputs allow more threads before diminishing returns

## Deliverables

- `benchmark_thread_scaling.cpp` - Comprehensive thread scaling benchmark
- `thread_scaling_results.csv` - Raw benchmark data
- `plot_performance.py` - Visualization script
- `plot_performance.gnuplot` - Fallback plotting script
- 4 graphs (PNG + PDF):
  - `time_vs_size.png/pdf`
  - `speedup_vs_threads.png/pdf`
  - `throughput_vs_threads.png/pdf`
  - `efficiency_vs_threads.png/pdf`
- Updated `WORK_SUMMARY.md`

## Dependencies

- C++ compiler with C++17 support
- CMake 3.10+
- ParlayLib (already installed via submodule)
- Python 3 + matplotlib (or gnuplot as fallback)
- Multi-core system for meaningful thread scaling tests

## Success Criteria

- ✓ All tests pass with parallel enabled
- ✓ Benchmark runs successfully for all configurations
- ✓ Results show expected behavior (serial faster than parallel-1, parallel scales with threads)
- ✓ All 4 graphs generated successfully
- ✓ Documentation updated with findings
- ✓ Code committed to repository
