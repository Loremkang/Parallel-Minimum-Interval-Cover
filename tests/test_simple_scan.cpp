#include "interval_covering.h"
#include <iostream>
#include <vector>
#include <random>

parlay::sequence<std::pair<int, int>> generate_intervals(size_t n, int seed = 42) {
  std::mt19937 rng(seed);
  std::uniform_int_distribution<size_t> step_dist(1, 10);
  std::uniform_int_distribution<size_t> len_dist(5, 20);

  parlay::sequence<size_t> lefts = parlay::tabulate(n, [&](size_t) { return step_dist(rng); });
  parlay::scan_inplace(lefts);
  parlay::sequence<size_t> rights = parlay::tabulate(n, [&](size_t i) { return lefts[i] + len_dist(rng); });

  parlay::sequence<std::pair<int, int>> intervals = parlay::tabulate(n, [&](size_t i) {
    return std::make_pair(static_cast<int>(lefts[i]), static_cast<int>(rights[i]));
  });

  return intervals;
}

int main() {
  std::cout << "Testing benchmark flow (serial then parallel)\n";

  const size_t n = 1000;
  auto intervals = generate_intervals(n);

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  // Test Serial Version (like benchmark)
  std::cout << "Running serial version...\n";
  std::cout.flush();
  {
    IntervalCovering solver(intervals.size(), getL, getR);
    solver.valid = parlay::sequence<bool>(n, 0);
    solver.KernelSerial();

    size_t count = 0;
    for (auto v : solver.valid) {
      count += v;
    }
    std::cout << "Serial selected " << count << " intervals\n";
  }

  // Test Parallel Version (like benchmark)
  std::cout << "Running parallel version...\n";
  std::cout.flush();
  {
    IntervalCovering solver(intervals.size(), getL, getR);
    solver.Run();

    size_t count = 0;
    for (auto v : solver.valid) {
      count += v;
    }
    std::cout << "Parallel selected " << count << " intervals\n";
  }

  std::cout << "Both versions completed!\n";

  return 0;
}
