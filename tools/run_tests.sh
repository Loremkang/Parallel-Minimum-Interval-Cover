#!/bin/bash

# Unified Benchmark Runner
# This script runs all benchmarks and generates all visualizations

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."

echo "=========================================="
echo "   PARALLEL INTERVAL COVERING BENCHMARKS"
echo "=========================================="
echo ""

# Check if build directory exists
if [ ! -d "$PROJECT_ROOT/build/bin" ]; then
    echo "Error: Build directory not found"
    echo "Please run: cd build && cmake .. && ninja"
    exit 1
fi

# Change to project root for consistent paths
cd "$PROJECT_ROOT"

# Clean up old results
echo "Cleaning up old results..."
rm -f thread_scaling_results.csv parallel_breakdown.csv
echo ""

# Run thread scaling benchmark
echo "=========================================="
echo "Step 1: Running Thread Scaling Benchmark"
echo "=========================================="
echo ""
bash "$SCRIPT_DIR/run_thread_scaling.sh"
if [ $? -ne 0 ]; then
    echo "Error: Thread scaling benchmark failed"
    exit 1
fi
echo ""

# Run parallel breakdown benchmark
echo "=========================================="
echo "Step 2: Running Parallel Breakdown Benchmark"
echo "=========================================="
echo ""
bash "$SCRIPT_DIR/run_parallel_breakdown.sh"
if [ $? -ne 0 ]; then
    echo "Error: Parallel breakdown benchmark failed"
    exit 1
fi
echo ""

# Generate all plots
echo "=========================================="
echo "Step 3: Generating Visualizations"
echo "=========================================="
echo ""

if [ -f "$SCRIPT_DIR/plot_results.py" ]; then
    python3 "$SCRIPT_DIR/plot_results.py"
    if [ $? -ne 0 ]; then
        echo "Warning: Unified plotting script failed, trying individual scripts..."
        python3 "$SCRIPT_DIR/plot_performance.py"
        python3 "$SCRIPT_DIR/plot_breakdown.py"
    fi
else
    echo "Running individual plotting scripts..."
    python3 "$SCRIPT_DIR/plot_performance.py"
    python3 "$SCRIPT_DIR/plot_breakdown.py"
fi

echo ""
echo "=========================================="
echo "All benchmarks complete!"
echo "=========================================="
echo ""
echo "Results:"
echo "  - thread_scaling_results.csv"
echo "  - parallel_breakdown.csv"
echo ""
echo "Visualizations saved in plots/ directory"
echo ""
