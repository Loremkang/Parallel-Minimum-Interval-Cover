#include "interval_covering.h"
#include "test_utils.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std::chrono;

struct BreakdownResult {
  size_t n;
  size_t threads;
  double build_furthest_ms;
  double sample_intervals_ms;
  double build_connections_ms;
  double scan_samples_ms;
  double scan_nonsample_ms;
  double total_ms;
};

// Run KernelParallelFast with timing for each phase
template<typename GetL, typename GetR>
BreakdownResult RunKernelParallelWithTiming(IntervalCovering<GetL, GetR>& solver) {
  BreakdownResult result;
  result.n = solver.n;
  result.threads = parlay::num_workers();

  auto start_total = high_resolution_clock::now();

  // Phase 1: BuildFurthest
  auto start = high_resolution_clock::now();
  solver.BuildFurthest();
  auto end = high_resolution_clock::now();
  result.build_furthest_ms = duration_cast<microseconds>(end - start).count() / 1000.0;

  // Phase 2: BuildIntervalSample
  start = high_resolution_clock::now();
  solver.BuildIntervalSample();
  end = high_resolution_clock::now();
  result.sample_intervals_ms = duration_cast<microseconds>(end - start).count() / 1000.0;

  // Phase 3: BuildConnectionBetweenSamples
  start = high_resolution_clock::now();
  solver.BuildConnectionBetweenSamples();
  end = high_resolution_clock::now();
  result.build_connections_ms = duration_cast<microseconds>(end - start).count() / 1000.0;

  // Phase 4: ScanSamples
  start = high_resolution_clock::now();
  solver.ScanSamples();
  end = high_resolution_clock::now();
  result.scan_samples_ms = duration_cast<microseconds>(end - start).count() / 1000.0;

  // Phase 5: ScanNonsampleNodes
  start = high_resolution_clock::now();
  solver.ScanNonsampleNodes();
  end = high_resolution_clock::now();
  result.scan_nonsample_ms = duration_cast<microseconds>(end - start).count() / 1000.0;

  auto end_total = high_resolution_clock::now();
  result.total_ms = duration_cast<microseconds>(end_total - start_total).count() / 1000.0;

  return result;
}

BreakdownResult run_breakdown_benchmark(size_t n, int num_runs = 3) {
  auto intervals = test_utils::generate_intervals(n);
  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  parlay::sequence<BreakdownResult> results;

  for (int run = 0; run < num_runs; run++) {
    IntervalCovering solver(intervals.size(), getL, getR);
    solver.valid = parlay::sequence<bool>(n, 0);

    auto result = RunKernelParallelWithTiming(solver);
    results.push_back(result);
  }

  // Return median result based on total time
  std::sort(results.begin(), results.end(),
            [](const BreakdownResult& a, const BreakdownResult& b) {
              return a.total_ms < b.total_ms;
            });

  return results[num_runs / 2];
}

int main(int argc, char* argv[]) {
  std::cout << "Parallel Algorithm Breakdown Benchmark\n";
  std::cout << "=======================================\n\n";

  size_t current_threads = parlay::num_workers();
  std::cout << "Threads: " << current_threads << "\n\n";

  // Test configurations - can be overridden via command line
  parlay::sequence<size_t> sizes = {10000, 100000, 1000000, 10000000};

  // Parse command line arguments for custom sizes
  if (argc > 1) {
    sizes.clear();
    for (int i = 1; i < argc; i++) {
      sizes.push_back(std::stoull(argv[i]));
    }
  }

  parlay::sequence<BreakdownResult> results;

  std::cout << std::setw(12) << "N"
            << std::setw(14) << "BuildFurthest"
            << std::setw(14) << "SampleInterv"
            << std::setw(14) << "BuildConnect"
            << std::setw(12) << "ScanSample"
            << std::setw(14) << "ScanNonsample"
            << std::setw(12) << "Total"
            << "\n";
  std::cout << std::string(92, '-') << "\n";

  for (size_t n : sizes) {
    std::cout << "Running n=" << n << "..." << std::flush;
    auto result = run_breakdown_benchmark(n);
    results.push_back(result);

    std::cout << "\r";
    std::cout << std::setw(12) << n
              << std::setw(14) << std::fixed << std::setprecision(2) << result.build_furthest_ms
              << std::setw(14) << result.sample_intervals_ms
              << std::setw(14) << result.build_connections_ms
              << std::setw(12) << result.scan_samples_ms
              << std::setw(14) << result.scan_nonsample_ms
              << std::setw(12) << result.total_ms
              << std::endl;

    if (result.total_ms > 30000) {
      std::cout << "\nBenchmark taking too long, stopping.\n";
      break;
    }
  }

  // Append results to CSV (allows multiple runs with different thread counts)
  bool file_exists = std::ifstream("parallel_breakdown.csv").good();
  std::ofstream csv("parallel_breakdown.csv", std::ios::app);

  if (!file_exists) {
    csv << "n,threads,build_furthest_ms,sample_intervals_ms,build_connections_ms,scan_samples_ms,scan_nonsample_ms,total_ms\n";
  }

  for (const auto& r : results) {
    csv << r.n << "," << r.threads << ","
        << std::fixed << std::setprecision(4)
        << r.build_furthest_ms << ","
        << r.sample_intervals_ms << ","
        << r.build_connections_ms << ","
        << r.scan_samples_ms << ","
        << r.scan_nonsample_ms << ","
        << r.total_ms << "\n";
  }
  csv.close();

  std::cout << "\n=======================================\n";
  std::cout << "Results appended to parallel_breakdown.csv\n";

  return 0;
}
