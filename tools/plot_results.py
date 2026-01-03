#!/usr/bin/env python3

"""
Unified Results Plotting Script
Generates all visualizations from benchmark data
"""

import sys
import os
from pathlib import Path

def main():
    print("=" * 70)
    print("   GENERATING ALL VISUALIZATIONS")
    print("=" * 70)
    print()

    # Get script directory
    script_dir = Path(__file__).parent

    # Check for required data files
    required_files = [
        'thread_scaling_results.csv',
        'parallel_breakdown.csv'
    ]

    missing_files = []
    for file in required_files:
        if not Path(file).exists():
            missing_files.append(file)

    if missing_files:
        print("Error: Missing required data files:")
        for file in missing_files:
            print(f"  - {file}")
        print()
        print("Please run the benchmarks first:")
        print("  bash tools/run_tests.sh")
        sys.exit(1)

    # Import and run individual plotting scripts
    print("Step 1: Generating thread scaling visualizations...")
    print("-" * 70)
    try:
        # Change to script directory to ensure proper imports
        original_dir = os.getcwd()
        os.chdir(script_dir)

        # Import and execute plot_performance
        import importlib.util
        spec = importlib.util.spec_from_file_location(
            "plot_performance",
            script_dir / "plot_performance.py"
        )
        plot_performance = importlib.util.module_from_spec(spec)

        # Execute the module
        with open(script_dir / "plot_performance.py") as f:
            code = f.read()

        # Go back to original directory for data access
        os.chdir(original_dir)
        exec(code, {'__name__': '__main__'})

        print()
        print("✓ Thread scaling visualizations complete")
        print()

    except Exception as e:
        print(f"Warning: Error generating thread scaling plots: {e}")
        print()

    print("Step 2: Generating parallel breakdown visualizations...")
    print("-" * 70)
    try:
        os.chdir(script_dir)

        # Execute plot_breakdown
        with open(script_dir / "plot_breakdown.py") as f:
            code = f.read()

        os.chdir(original_dir)
        exec(code, {'__name__': '__main__'})

        print()
        print("✓ Parallel breakdown visualizations complete")
        print()

    except Exception as e:
        print(f"Warning: Error generating breakdown plots: {e}")
        print()

    print("=" * 70)
    print("All visualizations generated successfully!")
    print("=" * 70)
    print()
    print("Output directory: plots/")
    print()
    print("Generated files:")

    # List generated plot files
    plots_dir = Path('plots')
    if plots_dir.exists():
        plot_files = sorted(plots_dir.glob('*.png'))
        for i, plot_file in enumerate(plot_files, 1):
            print(f"  {i}. {plot_file}")

    print()

if __name__ == '__main__':
    main()
