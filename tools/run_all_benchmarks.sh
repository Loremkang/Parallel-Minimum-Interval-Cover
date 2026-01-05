#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "========================================"
echo "Running ALL Benchmarks"
echo "========================================"
echo ""

echo "### Parallel Breakdown Benchmark ###"
"$SCRIPT_DIR/bench_parallel_breakdown.sh"

echo ""
echo "----------------------------------------"
echo ""

echo "### Thread Scaling Benchmark ###"
"$SCRIPT_DIR/bench_thread_scaling.sh"

echo ""
echo "========================================"
echo "All benchmarks complete!"
echo "  Results: results/"
echo "  Plots:   plots/"
echo "========================================"
