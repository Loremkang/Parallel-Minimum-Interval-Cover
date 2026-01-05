#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "========================================="
echo "Thread Scaling Benchmark"
echo "========================================="

# Step 1: Recompile project
echo ""
echo "[1/3] Recompiling project..."
cd "$PROJECT_ROOT/build"
cmake .. && make benchmark_thread_scaling
cd - > /dev/null

# Step 2: Run benchmark
echo ""
echo "[2/3] Running benchmark..."
"$SCRIPT_DIR/run_thread_scaling.sh"

# Step 3: Generate plots
echo ""
echo "[3/3] Generating plots..."
python3 "$SCRIPT_DIR/plot_thread_scaling.py"

echo ""
echo "========================================="
echo "Complete!"
echo "  Results: results/thread_scaling.csv"
echo "  Plots:   plots/{time,speedup,throughput,efficiency}_*.{png,pdf}"
echo "========================================="
