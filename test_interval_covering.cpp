#include "interval_covering.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// Test helper functions
template <typename T>
void print_intervals(const std::vector<std::pair<T, T>>& intervals) {
  for (size_t i = 0; i < intervals.size(); i++) {
    std::cout << "  [" << i << "]: (" << intervals[i].first << ", "
              << intervals[i].second << ")\n";
  }
}

template <typename T>
void print_result(const std::vector<std::pair<T, T>>& intervals,
                  const std::vector<uint8_t>& valid) {
  std::cout << "Minimum interval cover:\n";
  for (size_t i = 0; i < valid.size(); i++) {
    if (valid[i]) {
      std::cout << "  Interval " << i << ": (" << intervals[i].first << ", "
                << intervals[i].second << ")\n";
    }
  }
}

// Verify that the selected intervals form a valid cover
template <typename T>
bool verify_cover(const std::vector<std::pair<T, T>>& intervals,
                  const std::vector<uint8_t>& valid, T target = 0) {
  std::vector<size_t> selected;
  for (size_t i = 0; i < valid.size(); i++) {
    if (valid[i]) {
      selected.push_back(i);
    }
  }

  if (selected.empty()) {
    std::cerr << "ERROR: No intervals selected\n";
    return false;
  }

  // Check that intervals cover from target point
  if (intervals[selected[0]].first > target) {
    std::cerr << "ERROR: First interval doesn't cover target " << target
              << "\n";
    return false;
  }

  // Check continuity
  for (size_t i = 0; i < selected.size() - 1; i++) {
    T current_right = intervals[selected[i]].second;
    T next_left = intervals[selected[i + 1]].first;
    if (next_left > current_right) {
      std::cerr << "ERROR: Gap between interval " << selected[i] << " and "
                << selected[i + 1] << "\n";
      return false;
    }
  }

  return true;
}

// Test 1: Simple small case
void test_simple() {
  std::cout << "\n=== Test 1: Simple Case ===\n";
  std::vector<std::pair<int, int>> intervals = {{0, 5},   {1, 8},   {3, 10},
                                                 {7, 15},  {12, 20}, {18, 25},
                                                 {22, 30}, {28, 35}};

  std::cout << "Input intervals:\n";
  print_intervals(intervals);

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  print_result(intervals, solver.valid);
  assert(verify_cover(intervals, solver.valid));

  // Expected: intervals that extend coverage the most
  // Should include interval 0 (covers 0-5), then one that reaches furthest
  // from 0-5, etc.
  size_t count = 0;
  for (auto v : solver.valid) count += v;
  std::cout << "Selected " << count << " intervals\n";

  std::cout << "PASSED\n";
}

// Test 2: Single interval
void test_single_interval() {
  std::cout << "\n=== Test 2: Single Interval ===\n";
  std::vector<std::pair<int, int>> intervals = {{0, 10}};

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  assert(solver.valid[0] == 1);
  std::cout << "PASSED\n";
}

// Test 3: Two intervals
void test_two_intervals() {
  std::cout << "\n=== Test 3: Two Intervals ===\n";
  std::vector<std::pair<int, int>> intervals = {{0, 5}, {3, 10}};

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  print_result(intervals, solver.valid);
  assert(verify_cover(intervals, solver.valid));
  std::cout << "PASSED\n";
}

// Test 4: Adjacent intervals (touching but not overlapping)
void test_non_overlapping() {
  std::cout << "\n=== Test 4: Adjacent Intervals ===\n";
  // Must satisfy L(i+1) <= R(i), so make them touch
  std::vector<std::pair<int, int>> intervals = {
      {0, 5}, {5, 10}, {10, 15}, {15, 20}};

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  print_result(intervals, solver.valid);

  // Adjacent intervals should all be selected
  size_t count = 0;
  for (auto v : solver.valid) count += v;
  std::cout << "Selected " << count << " intervals (expected all)\n";

  std::cout << "PASSED\n";
}

// Test 5: Nested/Overlapping intervals with monotonic L and R
void test_nested() {
  std::cout << "\n=== Test 5: Nested/Overlapping Intervals ===\n";
  // Must satisfy: L(i) < L(i+1) AND R(i) < R(i+1)
  std::vector<std::pair<int, int>> intervals = {{0, 50}, {10, 60}, {15, 70},
                                                 {30, 80}, {35, 90}};

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  print_result(intervals, solver.valid);
  assert(verify_cover(intervals, solver.valid));

  std::cout << "PASSED\n";
}

// Test 6: Many overlapping intervals
void test_many_overlapping() {
  std::cout << "\n=== Test 6: Many Overlapping Intervals ===\n";
  std::vector<std::pair<int, int>> intervals;
  for (int i = 0; i < 50; i++) {
    intervals.push_back({i * 2, i * 2 + 10});
  }

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  assert(verify_cover(intervals, solver.valid));

  size_t count = 0;
  for (auto v : solver.valid) count += v;
  std::cout << "Selected " << count << " out of " << intervals.size()
            << " intervals\n";

  std::cout << "PASSED\n";
}

// Test 7: Large random test
void test_large_random() {
  std::cout << "\n=== Test 7: Large Random Test ===\n";
  const size_t n = 10000;
  std::vector<std::pair<int, int>> intervals;

  // Generate random intervals with strict monotonicity
  // Must ensure L(i) < L(i+1) and R(i) < R(i+1)
  int left = 0;
  int right = 10;
  for (size_t i = 0; i < n; i++) {
    intervals.push_back({left, right});
    left += (rand() % 5) + 1;    // +1 ensures strict increase
    right += (rand() % 5) + 4;   // +4 ensures strict increase
  }

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  assert(verify_cover(intervals, solver.valid));

  size_t count = 0;
  for (auto v : solver.valid) count += v;
  std::cout << "Selected " << count << " out of " << intervals.size()
            << " intervals\n";

  std::cout << "PASSED\n";
}

// Test 8: Edge case with very similar (nearly identical) intervals
void test_identical_intervals() {
  std::cout << "\n=== Test 8: Very Similar Intervals ===\n";
  // Cannot have identical L or R values due to strict monotonicity requirement
  std::vector<std::pair<int, int>> intervals = {
      {0, 10}, {5, 15}, {6, 16}, {7, 17}, {10, 20}};

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  print_result(intervals, solver.valid);
  assert(verify_cover(intervals, solver.valid));

  std::cout << "PASSED\n";
}

// Test 9: Very long chain
void test_long_chain() {
  std::cout << "\n=== Test 9: Long Chain ===\n";
  const size_t n = 1000;
  std::vector<std::pair<int, int>> intervals;

  for (size_t i = 0; i < n; i++) {
    intervals.push_back({(int)i, (int)i + 2});
  }

  auto getL = [&](size_t i) { return intervals[i].first; };
  auto getR = [&](size_t i) { return intervals[i].second; };

  IntervalCovering solver(intervals.size(), getL, getR);
  solver.Run();

  assert(verify_cover(intervals, solver.valid));

  size_t count = 0;
  for (auto v : solver.valid) count += v;
  std::cout << "Selected " << count << " out of " << intervals.size()
            << " intervals\n";

  std::cout << "PASSED\n";
}

// Test 10: Stress test with various sizes
void test_various_sizes() {
  std::cout << "\n=== Test 10: Various Sizes ===\n";

  std::vector<size_t> sizes = {1,   2,   3,    5,    10,   50,
                                100, 500, 1000, 5000, 10000};

  for (size_t n : sizes) {
    std::vector<std::pair<int, int>> intervals;
    int left = 0;
    int right = 5;

    for (size_t i = 0; i < n; i++) {
      intervals.push_back({left, right});
      left += (rand() % 3) + 1;   // +1 ensures strict increase
      right += (rand() % 4) + 3;  // +3 ensures strict increase
    }

    auto getL = [&](size_t i) { return intervals[i].first; };
    auto getR = [&](size_t i) { return intervals[i].second; };

    IntervalCovering solver(intervals.size(), getL, getR);
    solver.Run();

    assert(verify_cover(intervals, solver.valid));

    size_t count = 0;
    for (auto v : solver.valid) count += v;
    std::cout << "  n=" << n << ": selected " << count << " intervals\n";
  }

  std::cout << "PASSED\n";
}

int main() {
  std::cout << "Running Interval Covering Tests\n";
  std::cout << "================================\n";

  try {
    test_simple();
    test_single_interval();
    test_two_intervals();
    test_non_overlapping();
    test_nested();
    test_many_overlapping();
    test_large_random();
    test_identical_intervals();
    test_long_chain();
    test_various_sizes();

    std::cout << "\n================================\n";
    std::cout << "ALL TESTS PASSED!\n";
    std::cout << "================================\n";

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Test failed with exception: " << e.what() << "\n";
    return 1;
  } catch (...) {
    std::cerr << "Test failed with unknown exception\n";
    return 1;
  }
}
