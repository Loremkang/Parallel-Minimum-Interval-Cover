#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "========================================="
echo "Thread Scaling Benchmark"
echo "========================================="

# Step 1: Run benchmark
echo ""
echo "[1/2] Running benchmark..."
"$SCRIPT_DIR/run_thread_scaling.sh"

# Step 2: Generate plots
echo ""
echo "[2/2] Generating plots..."
python3 "$SCRIPT_DIR/plot_thread_scaling.py"

echo ""
echo "========================================="
echo "Complete!"
echo "  Results: results/thread_scaling.csv"
echo "  Plots:   plots/{time,speedup,throughput,efficiency}_*.{png,pdf}"
echo "========================================="
