#!/bin/bash

# Thread Scaling Benchmark Runner
# This script runs the benchmark with different thread counts

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"
BENCHMARK="$BUILD_DIR/benchmark_thread_scaling"

# Check if benchmark exists
if [ ! -f "$BENCHMARK" ]; then
    echo "Error: benchmark_thread_scaling not found in $BUILD_DIR"
    echo "Please run: cd build && cmake .. && make benchmark_thread_scaling"
    exit 1
fi

# Remove old results file
rm -f thread_scaling_results.csv

# Get system thread count
SYSTEM_THREADS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 8)
echo "System has $SYSTEM_THREADS threads"
echo ""

# Thread counts to test
THREAD_COUNTS="1 2 4 8 16 32"

# Filter thread counts to not exceed system max
FILTERED_COUNTS=""
for T in $THREAD_COUNTS; do
    if [ $T -le $SYSTEM_THREADS ]; then
        FILTERED_COUNTS="$FILTERED_COUNTS $T"
    fi
done

echo "Testing thread counts:$FILTERED_COUNTS"
echo "======================================"
echo ""

# Run benchmark for each thread count
for THREADS in $FILTERED_COUNTS; do
    echo "Running with $THREADS threads..."
    echo "--------------------------------------"
    PARLAY_NUM_THREADS=$THREADS $BENCHMARK
    echo ""
done

echo "======================================"
echo "Benchmarking complete!"
echo "Results saved to thread_scaling_results.csv"
echo ""
echo "To generate graphs, run:"
echo "  python3 plot_performance.py"
echo ""
