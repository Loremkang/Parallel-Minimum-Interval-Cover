#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "========================================="
echo "Parallel Breakdown Benchmark"
echo "========================================="

# Step 1: Recompile project
echo ""
echo "[1/3] Recompiling project..."
cd "$PROJECT_ROOT/build"
cmake -G Ninja .. && ninja benchmark_parallel_breakdown
cd - > /dev/null

# Step 2: Run benchmark
echo ""
echo "[2/3] Running benchmark..."
"$SCRIPT_DIR/run_parallel_breakdown.sh"

# Step 3: Generate plots
echo ""
echo "[3/3] Generating plots..."
python3 "$SCRIPT_DIR/plot_parallel_breakdown.py"

echo ""
echo "========================================="
echo "Complete!"
echo "  Results: results/parallel_breakdown.csv"
echo "  Plots:   plots/breakdown_*.{png,pdf}"
echo "========================================="
