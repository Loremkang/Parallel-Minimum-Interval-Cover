# Benchmark Scripts Refactor Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Refactor benchmark scripts to add NUMA binding and reorganize into modular run/plot/bench structure.

**Architecture:** Split existing scripts into three categories: `run_*.sh` (execute benchmarks with numactl), `plot_*.py` (generate graphs), and `bench_*.sh` (combine both). Standardize output to `results/` and `plots/` directories.

**Tech Stack:** Bash, Python 3, numactl, matplotlib

---

## Task 1: Create Directory Structure

**Files:**
- Verify: `results/` directory will be auto-created
- Verify: `plots/` directory already exists

**Step 1: Verify plots directory exists**

Run:
```bash
ls -ld plots/
```

Expected: Directory exists (created by current plot scripts)

**Step 2: Verify results directory will be created by scripts**

Note: The `results/` directory will be auto-created by the run scripts, no action needed here.

**Step 3: Continue to next task**

No commit needed - just verification.

---

## Task 2: Modify run_parallel_breakdown.sh

**Files:**
- Modify: `tools/run_parallel_breakdown.sh`

**Step 1: Back up current script**

Run:
```bash
cp tools/run_parallel_breakdown.sh tools/run_parallel_breakdown.sh.bak
```

Expected: Backup created

**Step 2: Replace script content**

Edit `tools/run_parallel_breakdown.sh` with:

```bash
#!/bin/bash
set -e  # Exit on error
set -u  # Error on undefined variables

# Configuration
NUMA_NODE=0  # Hardcoded NUMA node configuration

# Path setup (relative to project root)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/bin"
RESULTS_DIR="$PROJECT_ROOT/results"

# Create results directory
mkdir -p "$RESULTS_DIR"

# Check if numactl is available
if ! command -v numactl &> /dev/null; then
    echo "WARNING: numactl not found, running without NUMA binding"
    NUMACTL_CMD=""
else
    NUMACTL_CMD="numactl --cpunodebind=$NUMA_NODE --membind=$NUMA_NODE"
fi

# Check if benchmark exists
BENCHMARK="$BUILD_DIR/benchmark_parallel_breakdown"
if [ ! -f "$BENCHMARK" ]; then
    echo "Error: benchmark_parallel_breakdown not found at $BUILD_DIR"
    echo "Please run: cd build && cmake .. && ninja benchmark_parallel_breakdown"
    exit 1
fi

# Remove old results file
OUTPUT_FILE="$RESULTS_DIR/parallel_breakdown.csv"
rm -f "$OUTPUT_FILE"

# Get system thread count
SYSTEM_THREADS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 8)
echo "System has $SYSTEM_THREADS threads"
echo "NUMA binding: node $NUMA_NODE"
echo "Output: $OUTPUT_FILE"
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
    PARLAY_NUM_THREADS=$THREADS $NUMACTL_CMD $BENCHMARK
    echo ""
done

echo "======================================"
echo "Benchmarking complete!"
echo "Results saved to $OUTPUT_FILE"
echo ""
```

**Step 3: Make script executable**

Run:
```bash
chmod +x tools/run_parallel_breakdown.sh
```

Expected: Script is executable

**Step 4: Test script runs (optional - requires built benchmark)**

Run:
```bash
tools/run_parallel_breakdown.sh
```

Expected: Either runs successfully or shows clear error if benchmark not built

**Step 5: Commit**

```bash
git add tools/run_parallel_breakdown.sh
git commit -m "refactor(bench): add NUMA binding and results/ output to parallel breakdown

- Add NUMA_NODE variable (default: 0)
- Check numactl availability with fallback
- Output to results/parallel_breakdown.csv
- Add comprehensive error handling
- Use set -e and set -u for robustness"
```

---

## Task 3: Modify run_thread_scaling.sh

**Files:**
- Modify: `tools/run_thread_scaling.sh`

**Step 1: Replace script content**

Edit `tools/run_thread_scaling.sh` with:

```bash
#!/bin/bash
set -e  # Exit on error
set -u  # Error on undefined variables

# Configuration
NUMA_NODE=0  # Hardcoded NUMA node configuration

# Path setup (relative to project root)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/bin"
RESULTS_DIR="$PROJECT_ROOT/results"

# Create results directory
mkdir -p "$RESULTS_DIR"

# Check if numactl is available
if ! command -v numactl &> /dev/null; then
    echo "WARNING: numactl not found, running without NUMA binding"
    NUMACTL_CMD=""
else
    NUMACTL_CMD="numactl --cpunodebind=$NUMA_NODE --membind=$NUMA_NODE"
fi

# Check if benchmark exists
BENCHMARK="$BUILD_DIR/benchmark_thread_scaling"
if [ ! -f "$BENCHMARK" ]; then
    echo "Error: benchmark_thread_scaling not found at $BUILD_DIR"
    echo "Please run: cd build && cmake .. && make benchmark_thread_scaling"
    exit 1
fi

# Remove old results file
OUTPUT_FILE="$RESULTS_DIR/thread_scaling.csv"
rm -f "$OUTPUT_FILE"

# Get system thread count
SYSTEM_THREADS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 8)
echo "System has $SYSTEM_THREADS threads"
echo "NUMA binding: node $NUMA_NODE"
echo "Output: $OUTPUT_FILE"
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
    PARLAY_NUM_THREADS=$THREADS $NUMACTL_CMD $BENCHMARK
    echo ""
done

echo "======================================"
echo "Benchmarking complete!"
echo "Results saved to $OUTPUT_FILE"
echo ""
```

**Step 2: Make script executable**

Run:
```bash
chmod +x tools/run_thread_scaling.sh
```

**Step 3: Commit**

```bash
git add tools/run_thread_scaling.sh
git commit -m "refactor(bench): add NUMA binding and results/ output to thread scaling

- Add NUMA_NODE variable (default: 0)
- Check numactl availability with fallback
- Output to results/thread_scaling.csv
- Add comprehensive error handling
- Use set -e and set -u for robustness"
```

---

## Task 4: Rename and Modify plot_breakdown.py

**Files:**
- Rename: `tools/plot_breakdown.py` → `tools/plot_parallel_breakdown.py`
- Modify: `tools/plot_parallel_breakdown.py`

**Step 1: Rename file**

Run:
```bash
git mv tools/plot_breakdown.py tools/plot_parallel_breakdown.py
```

Expected: File renamed in git

**Step 2: Modify to use results/ and plots/ paths**

Edit `tools/plot_parallel_breakdown.py`, replace lines 1-14 with:

```python
#!/usr/bin/env python3

import csv
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from pathlib import Path
import sys

# Use a non-interactive backend if no display available
matplotlib.use('Agg')

# Path setup
SCRIPT_DIR = Path(__file__).parent
PROJECT_ROOT = SCRIPT_DIR.parent
RESULTS_DIR = PROJECT_ROOT / 'results'
PLOTS_DIR = PROJECT_ROOT / 'plots'

# Validate input exists
INPUT_FILE = RESULTS_DIR / 'parallel_breakdown.csv'
if not INPUT_FILE.exists():
    print(f"Error: {INPUT_FILE} not found")
    print("Please run: tools/run_parallel_breakdown.sh")
    sys.exit(1)

# Create plots directory
PLOTS_DIR.mkdir(exist_ok=True)

# Read the CSV data
data = []
with open(INPUT_FILE, 'r') as f:
    reader = csv.DictReader(f)
```

**Step 3: Update all plot save paths**

Replace lines 40-41 with:
```python
# No change needed - plots/ directory already used
```

And replace line 94-95 with:
```python
plt.savefig(PLOTS_DIR / 'breakdown_stacked.png', dpi=300, bbox_inches='tight')
plt.savefig(PLOTS_DIR / 'breakdown_stacked.pdf', bbox_inches='tight')
print(f"  Saved: {PLOTS_DIR / 'breakdown_stacked.png'}\n")
```

Similarly update lines 130-132, 174-176, 222-224 to use `PLOTS_DIR /` instead of `'plots/'`.

**Step 4: Make script executable**

Run:
```bash
chmod +x tools/plot_parallel_breakdown.py
```

**Step 5: Commit**

```bash
git add tools/plot_parallel_breakdown.py
git commit -m "refactor(bench): rename and update plot_breakdown.py paths

- Rename plot_breakdown.py → plot_parallel_breakdown.py
- Read from results/parallel_breakdown.csv
- Output to plots/ directory
- Add input file validation
- Ensure plots directory exists"
```

---

## Task 5: Rename and Modify plot_performance.py

**Files:**
- Rename: `tools/plot_performance.py` → `tools/plot_thread_scaling.py`
- Modify: `tools/plot_thread_scaling.py`

**Step 1: Rename file**

Run:
```bash
git mv tools/plot_performance.py tools/plot_thread_scaling.py
```

**Step 2: Modify to use results/ and plots/ paths**

Edit `tools/plot_thread_scaling.py`, replace lines 1-14 with:

```python
#!/usr/bin/env python3

import csv
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from pathlib import Path
import sys

# Use a non-interactive backend if no display available
matplotlib.use('Agg')

# Path setup
SCRIPT_DIR = Path(__file__).parent
PROJECT_ROOT = SCRIPT_DIR.parent
RESULTS_DIR = PROJECT_ROOT / 'results'
PLOTS_DIR = PROJECT_ROOT / 'plots'

# Validate input exists
INPUT_FILE = RESULTS_DIR / 'thread_scaling.csv'
if not INPUT_FILE.exists():
    print(f"Error: {INPUT_FILE} not found")
    print("Please run: tools/run_thread_scaling.sh")
    sys.exit(1)

# Create plots directory
PLOTS_DIR.mkdir(exist_ok=True)

# Read the CSV data
data = []
with open(INPUT_FILE, 'r') as f:
    reader = csv.DictReader(f)
```

**Step 3: Update all plot save paths**

Replace lines 40-41 with:
```python
# plots/ directory already correct, just ensure PLOTS_DIR is used
```

Update lines 86-88, 126-128, 160-162, 201-203 to use `PLOTS_DIR /` instead of `'plots/'`:

Example for lines 86-88:
```python
plt.savefig(PLOTS_DIR / 'time_vs_size.png', dpi=300, bbox_inches='tight')
plt.savefig(PLOTS_DIR / 'time_vs_size.pdf', bbox_inches='tight')
print(f"  Saved: {PLOTS_DIR / 'time_vs_size.png'}\n")
```

**Step 4: Make script executable**

Run:
```bash
chmod +x tools/plot_thread_scaling.py
```

**Step 5: Commit**

```bash
git add tools/plot_thread_scaling.py
git commit -m "refactor(bench): rename and update plot_performance.py paths

- Rename plot_performance.py → plot_thread_scaling.py
- Read from results/thread_scaling.csv
- Output to plots/ directory
- Add input file validation
- Ensure plots directory exists"
```

---

## Task 6: Create bench_parallel_breakdown.sh

**Files:**
- Create: `tools/bench_parallel_breakdown.sh`

**Step 1: Create script**

Create `tools/bench_parallel_breakdown.sh` with:

```bash
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "========================================="
echo "Parallel Breakdown Benchmark"
echo "========================================="

# Step 1: Run benchmark
echo ""
echo "[1/2] Running benchmark..."
"$SCRIPT_DIR/run_parallel_breakdown.sh"

# Step 2: Generate plots
echo ""
echo "[2/2] Generating plots..."
python3 "$SCRIPT_DIR/plot_parallel_breakdown.py"

echo ""
echo "========================================="
echo "Complete!"
echo "  Results: results/parallel_breakdown.csv"
echo "  Plots:   plots/breakdown_*.{png,pdf}"
echo "========================================="
```

**Step 2: Make script executable**

Run:
```bash
chmod +x tools/bench_parallel_breakdown.sh
```

**Step 3: Test script (optional)**

Run:
```bash
tools/bench_parallel_breakdown.sh
```

Expected: Runs benchmark then generates plots, or shows clear error

**Step 4: Commit**

```bash
git add tools/bench_parallel_breakdown.sh
git commit -m "feat(bench): add combined parallel breakdown script

- Create bench_parallel_breakdown.sh
- Runs benchmark then generates plots
- Clear progress indication
- Summary of output locations"
```

---

## Task 7: Create bench_thread_scaling.sh

**Files:**
- Create: `tools/bench_thread_scaling.sh`

**Step 1: Create script**

Create `tools/bench_thread_scaling.sh` with:

```bash
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
```

**Step 2: Make script executable**

Run:
```bash
chmod +x tools/bench_thread_scaling.sh
```

**Step 3: Commit**

```bash
git add tools/bench_thread_scaling.sh
git commit -m "feat(bench): add combined thread scaling script

- Create bench_thread_scaling.sh
- Runs benchmark then generates plots
- Clear progress indication
- Summary of output locations"
```

---

## Task 8: Create run_all_benchmarks.sh

**Files:**
- Create: `tools/run_all_benchmarks.sh`

**Step 1: Create script**

Create `tools/run_all_benchmarks.sh` with:

```bash
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
```

**Step 2: Make script executable**

Run:
```bash
chmod +x tools/run_all_benchmarks.sh
```

**Step 3: Test script (optional)**

Run:
```bash
tools/run_all_benchmarks.sh
```

Expected: Runs all benchmarks sequentially

**Step 4: Commit**

```bash
git add tools/run_all_benchmarks.sh
git commit -m "feat(bench): add master script to run all benchmarks

- Create run_all_benchmarks.sh
- Runs both parallel breakdown and thread scaling
- Clear section headers
- Final summary"
```

---

## Task 9: Clean Up and Documentation

**Files:**
- Remove: `tools/run_parallel_breakdown.sh.bak` (if exists)
- Update: `README.md` or benchmark docs (if needed)

**Step 1: Remove backup files**

Run:
```bash
rm -f tools/run_parallel_breakdown.sh.bak
```

**Step 2: Verify all new scripts exist**

Run:
```bash
ls -l tools/{run,plot,bench}*.{sh,py}
```

Expected: All new scripts listed

**Step 3: Create results/.gitignore**

Create `results/.gitignore` with:
```
# Ignore all CSV result files
*.csv

# But track the directory
!.gitignore
```

**Step 4: Create results directory placeholder**

Run:
```bash
mkdir -p results
```

**Step 5: Commit**

```bash
git add results/.gitignore
git commit -m "chore(bench): add results directory with gitignore

- Create results/ directory
- Ignore CSV files in results/
- Keep directory tracked"
```

---

## Task 10: Final Verification

**Files:**
- Verify all scripts

**Step 1: List all benchmark scripts**

Run:
```bash
ls -lh tools/{run,bench,plot}*.{sh,py} 2>/dev/null || true
```

Expected output should show:
- `run_parallel_breakdown.sh`
- `run_thread_scaling.sh`
- `plot_parallel_breakdown.py`
- `plot_thread_scaling.py`
- `bench_parallel_breakdown.sh`
- `bench_thread_scaling.sh`
- `run_all_benchmarks.sh`

**Step 2: Verify directory structure**

Run:
```bash
tree -L 1 -d . | grep -E '(results|plots|tools)'
```

Expected:
- `results/` exists
- `plots/` exists
- `tools/` exists

**Step 3: Summary**

Print summary:
```bash
echo "Refactoring complete!"
echo "New structure:"
echo "  - Run scripts: tools/run_*.sh (with NUMA binding)"
echo "  - Plot scripts: tools/plot_*.py (results/ → plots/)"
echo "  - Bench scripts: tools/bench_*.sh (run + plot)"
echo "  - Master script: tools/run_all_benchmarks.sh"
echo ""
echo "Usage:"
echo "  Run only:  tools/run_parallel_breakdown.sh"
echo "  Plot only: tools/plot_parallel_breakdown.py"
echo "  Both:      tools/bench_parallel_breakdown.sh"
echo "  All:       tools/run_all_benchmarks.sh"
```

**No commit needed - this is just verification**

---

## Summary

This plan refactors the benchmark testing infrastructure:

1. ✅ Added NUMA binding to run scripts (default node 0)
2. ✅ Standardized output to `results/` directory
3. ✅ Updated plot scripts to read from `results/`, write to `plots/`
4. ✅ Created modular bench scripts that combine run + plot
5. ✅ Created master script to run all benchmarks
6. ✅ Added comprehensive error handling and validation

**Total commits:** 10
**Estimated time:** 30-45 minutes
