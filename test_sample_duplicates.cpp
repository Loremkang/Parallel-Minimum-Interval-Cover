#include <iostream>
#include <unordered_set>
#include "parlay/parallel.h"
#include "parlay/primitives.h"

int main() {
  // Test if BuildSampleId can generate duplicate node IDs
  size_t n = 1000;
  size_t nn = n * 2;
  size_t parallel_block_size = parlay::internal::_block_size;

  parlay::random rnd(0);
  size_t total_sampled = 1 + (nn + parallel_block_size - 1) / parallel_block_size;

  std::cout << "n = " << n << std::endl;
  std::cout << "nn = " << nn << std::endl;
  std::cout << "parallel_block_size = " << parallel_block_size << std::endl;
  std::cout << "total_sampled = " << total_sampled << std::endl;

  parlay::sequence<size_t> sampled_id(total_sampled);
  sampled_id[0] = (n - 1) * 2;  // l_nodeid(n-1)

  std::unordered_set<size_t> seen;
  seen.insert(sampled_id[0]);

  int duplicates = 0;
  for (size_t i = 1; i < total_sampled; i++) {
    sampled_id[i] = rnd.ith_rand(i) % nn;
    if (seen.count(sampled_id[i])) {
      duplicates++;
      std::cout << "Duplicate found at index " << i << ": node_id = "
                << sampled_id[i] << std::endl;
    }
    seen.insert(sampled_id[i]);
  }

  std::cout << "\nTotal duplicates: " << duplicates << std::endl;
  std::cout << "Unique nodes: " << seen.size() << std::endl;

  return 0;
}
