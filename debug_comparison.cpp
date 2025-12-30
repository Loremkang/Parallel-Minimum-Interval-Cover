#include "interval_covering.h"
#include <iostream>
#include <random>
#include <vector>

// Same generation as benchmark
std::vector<std::pair<int, int>> generate_intervals(size_t n, int seed = 42) {
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> step_dist(1, 10);
  std::uniform_int_distribution<int> len_dist(5, 20);

  std::vector<std::pair<int, int>> intervals;
  intervals.reserve(n);

  int left = 0;
  int right = len_dist(rng);

  for (size_t i = 0; i < n; i++) {
    intervals.push_back({left, right});
    left += step_dist(rng);
    right = left + len_dist(rng);
  }

  return intervals;
}

int main() {
  size_t n = 20;
  auto intervals = generate_intervals(n);

  std::cout << "Generated intervals:\n";
  for (size_t i = 0; i < n; i++) {
    std::cout << "  " << i << ": [" << intervals[i].first << ", "
              << intervals[i].second << "]\n";
  }

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  // Test Serial
  std::vector<size_t> serial_selected;
  {
    IntervalCovering solver(intervals.size(), getL, getR);
    solver.valid = std::vector<uint8_t>(n, 0);
    solver.KernelSerial();
    for (size_t i = 0; i < n; i++) {
      if (solver.valid[i]) {
        serial_selected.push_back(i);
      }
    }
  }

  // Test Parallel
  std::vector<size_t> parallel_selected;
  {
    IntervalCovering solver(intervals.size(), getL, getR);
    solver.Run();
    for (size_t i = 0; i < n; i++) {
      if (solver.valid[i]) {
        parallel_selected.push_back(i);
      }
    }
  }

  std::cout << "\nSerial selected " << serial_selected.size()
            << " intervals: ";
  for (auto idx : serial_selected) {
    std::cout << idx << " ";
  }
  std::cout << "\n";

  std::cout << "Parallel selected " << parallel_selected.size()
            << " intervals: ";
  for (auto idx : parallel_selected) {
    std::cout << idx << " ";
  }
  std::cout << "\n";

  return 0;
}
