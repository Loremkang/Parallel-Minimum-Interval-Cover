#ifndef INTERVAL_COVERING_H
#define INTERVAL_COVERING_H

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

#include "macro.h"
#include "parlay/parallel.h"
#include "parlay/primitives.h"

template<typename GetL, typename GetR>
class IntervalCovering {
 public:
  // Constructor
  IntervalCovering(size_t n, GetL L, GetR R)
      : n(n), L(L), R(R), furthest_id(n) {}

  struct LinkListNode {
    size_t nxt : 62;      // Next pointer (62 bits)
    size_t sampled : 1;   // Sampled flag (1 bit)
    size_t valid : 1;     // Valid flag (1 bit)

    LinkListNode(bool valid = false, size_t nxt = kNullPtr, bool sampled = false)
        : nxt(nxt), sampled(sampled), valid(valid) {}

    bool get_valid() const { return valid; }
    void set_valid(bool v) { valid = v; }

    bool get_sampled() const { return sampled; }
    void set_sampled(bool s) { sampled = s; }

    size_t get_nxt() const { return nxt; }
    void set_nxt(size_t n) { nxt = n; }
  };

  size_t l_nodeid(size_t i) const { return i * 2; }
  size_t r_nodeid(size_t i) const { return i * 2 + 1; }
  LinkListNode& l_node(size_t i) { return link_list[l_nodeid(i)]; }
  LinkListNode& r_node(size_t i) { return link_list[r_nodeid(i)]; }
  void link(size_t from, size_t to) { link_list[from].set_nxt(to); }
  std::string node_str(size_t nodeid) {
    if (nodeid == kNullPtr) {
      return "nullptr";
    } else {
      size_t i = nodeid;
      bool is_l = (i % 2 == 0);
      size_t id = i / 2;
      return (is_l ? "l" : "r") + std::to_string(id);
    }
  }

  void FindFurthestSerial(size_t s, size_t e) {
    // Binary search for the first segment
    size_t r_of_s = R(s);
    size_t l = s;
    size_t r = n;
    while (l + 1 < r) {
      size_t mid = (l + r) / 2;
      if (L(mid) <= r_of_s) {
        l = mid;
      } else {
        r = mid;
      }
    }
    furthest_id[s] = l;

    // Process remaining segments using monotonicity
    for (size_t j = s + 1; j < e; j++) {
      size_t rid = furthest_id[j - 1];
      size_t r_of_j = R(j);
      while (rid < n && L(rid) <= r_of_j) {
        rid++;
      }
      furthest_id[j] = rid - 1;
    }
  }

  void FindFurthestParallel() {
    parlay::internal::sliced_for(n, parallel_block_size,
                                 [&](size_t i, size_t s, size_t e) {
                                   (void)i;
                                   FindFurthestSerial(s, e);
                                 });
  }

  void FindFurthest() {
    FindFurthestParallel();

    DEBUG_ONLY {
      // Save parallel results
      std::vector<size_t> furthest_id_parallel(n);
      parlay::parallel_for(
          0, n, [&](size_t i) { furthest_id_parallel[i] = furthest_id[i]; });

      // Run serial version on a copy to verify
      std::vector<size_t> furthest_id_serial(n);
      std::swap(furthest_id, furthest_id_serial);
      FindFurthestSerial(0, n);

      // Verify results match
      for (size_t i = 0; i < n; i++) {
        assert(furthest_id[i] == furthest_id_parallel[i]);
      }

      // Restore parallel results
      furthest_id = furthest_id_parallel;
    }
  }

  // Build the link list for the euler tour
  void BuildLinkList() {
    link_list = std::vector<LinkListNode>(n * 2);

    r_node(0).set_valid(true);
    parlay::parallel_for(0, n - 1, [&](size_t i) {
      // set left node of l_node
      if (i == 0 || furthest_id[i - 1] != furthest_id[i]) {
        link(l_nodeid(furthest_id[i]), l_nodeid(i));
      } else {
        link(r_nodeid(i - 1), l_nodeid(i));
      }

      // set right node of r_node
      if (furthest_id[i + 1] != furthest_id[i]) {
        r_node(i).set_nxt(r_nodeid(furthest_id[i]));
      } else if (i + 1 == furthest_id[i]) {
        r_node(i).set_nxt(r_nodeid(i + 1));
      } else {
        // no need to set r_node(i).nxt, will be set by the other side
      }
    });

    // euler tour end points
    parlay::parallel_for(0, n, [&](size_t i) {
      if (l_node(i).get_nxt() == kNullPtr) {
        l_node(i).set_nxt(r_nodeid(i));
      }
    });

    // Set the last right node to terminate the list
    if (n > 0) {
      r_node(n - 1).set_nxt(kNullPtr);
    }

    VERBOSE_ONLY {
      {
        for (size_t i = 0; i < n; i++) {
          printf("%s -> %s, %s -> %s\n", node_str(l_nodeid(i)).c_str(),
                 node_str(l_node(i).get_nxt()).c_str(),
                 node_str(r_nodeid(i)).c_str(),
                 node_str(r_node(i).get_nxt()).c_str());
        }

        size_t nodeid = l_nodeid(n - 1);
        while (nodeid != kNullPtr) {
          printf("%s\n", node_str(nodeid).c_str());
          nodeid = link_list[nodeid].get_nxt();
        }
      }
    }

    DEBUG_ONLY {
      // check that start at l_nodeid(n-1), all nodes are reachable, and ends at r_nodeid(n - 1) before nullptr
      size_t nodeid = l_nodeid(n - 1);
      size_t count = 0;
      while (nodeid != r_nodeid(n - 1) && nodeid != kNullPtr) {
        count++;
        nodeid = link_list[nodeid].get_nxt();
      }
      assert(count == n * 2 - 1);
      assert(nodeid == r_nodeid(n - 1) && link_list[nodeid].get_nxt() == kNullPtr);
    }
  }

  void ScanLinkListSerial() {
    // Serial version: start from l_nodeid(n-1) and follow the link list
    size_t node_id = l_nodeid(n - 1);
    bool valid = false;
    while (node_id != kNullPtr) {
      valid = valid || link_list[node_id].valid;
      link_list[node_id].valid = valid;
      node_id = link_list[node_id].nxt;
    }
  }

  void BuildSampleId() {
    // sample n * 2 / parallel_block_size nodes from the link list
    size_t nn = n * 2;
    parlay::random rnd(0);

    size_t total_sampled = 1 + (nn + parallel_block_size - 1) / parallel_block_size;
    sampled_id.resize(total_sampled);
    sampled_id[0] = l_nodeid(n - 1);
    link_list[sampled_id[0]].set_sampled(true);
    for (size_t i = 1; i < total_sampled; i ++) {
      sampled_id[i] = rnd.ith_rand(i) % nn;
      link_list[sampled_id[i]].set_sampled(true);
    }
  }

  void ScanLinkListParallel(size_t segment_count = parallel_block_size) {
    (void)segment_count;  // Parameter not used yet
    BuildSampleId();
    
    // scan from each sampled node until next sampled node
    sampled_id_nxt.resize(sampled_id.size());
    parlay::parallel_for(0, sampled_id.size(), [&](size_t i) {
      size_t node_id = sampled_id[i];
      bool valid = false;
      while (node_id != kNullPtr) {
        valid = valid || link_list[node_id].valid;
        link_list[node_id].valid = valid;
        if (link_list[node_id].get_sampled()) {
          break;
        }
        node_id = link_list[node_id].nxt;
      }
      sampled_id_nxt[i] = node_id;
    });

    // scan over sampled sketch
    {
      size_t node_id = sampled_id[0];
      bool valid = false;
      while (node_id != kNullPtr) {
        valid = valid || link_list[node_id].valid;
        link_list[node_id].valid = valid;
        node_id = link_list[node_id].nxt;
      }
    }

    // scan from each sampled node until next sampled node
    parlay::parallel_for(0, sampled_id.size(), [&](size_t i) {
      size_t node_id = sampled_id[i];
      bool valid = false;
      while (node_id != kNullPtr) {
        valid = valid || link_list[node_id].valid;
        link_list[node_id].valid = valid;
        if (link_list[node_id].get_sampled()) {
          break;
        }
        node_id = link_list[node_id].nxt;
      }
      sampled_id_nxt[i] = node_id;
    });
  }

  void ScanLinkList() {
    DEBUG_ONLY {
      size_t nn = n * 2;

      // Save the original link_list valid flags
      std::vector<uint8_t> saved_valid(nn);
      parlay::parallel_for(
          0, nn, [&](size_t i) { saved_valid[i] = link_list[i].get_valid(); });

      // Run parallel version
      ScanLinkListParallel();

      // Save parallel results
      parlay::sequence<bool> parallel_valid(nn);
      parlay::parallel_for(
          0, nn, [&](size_t i) { parallel_valid[i] = link_list[i].get_valid(); });

      // Restore original state
      parlay::parallel_for(
          0, nn, [&](size_t i) { link_list[i].set_valid(saved_valid[i]); });

      // Run serial version
      ScanLinkListSerial();

      // Verify results match
      parlay::parallel_for(0, nn, [&](size_t i) {
        if (link_list[i].get_valid() != parallel_valid[i]) {
          printf("ScanLinkList mismatch at %ld: serial=%d, parallel=%d\n", i,
                 link_list[i].get_valid(), parallel_valid[i]);
          assert(false);
        }
      });

      return;
    }

    // Non-debug mode: just run parallel version
    ScanLinkListParallel();
  }

  void KernelParallel() {
    FindFurthest();
    BuildLinkList();
    ScanLinkList();
    parlay::parallel_for(0, n, [&](size_t i) {
      valid[i] = (l_node(i).get_valid() != r_node(i).get_valid()) ? 1 : 0;
    });
  }

  void KernelSerial() {
    valid[0] = 1;
    valid[n - 1] = 1;

    size_t id = 0;
    for (size_t i = 1; i < n - 1; i++) {
      if (L(i + 1) > R(id)) {
        valid[i] = 1;
        id = i;
      } else {
        valid[i] = 0;
      }
    }
  }

  void Kernel() {
    KernelParallel();

    DEBUG_ONLY {
      // Save parallel results
      parlay::sequence<uint8_t> valid_parallel(n);
      parlay::parallel_for(0, n, [&](size_t i) { valid_parallel[i] = valid[i]; });

      // Reset valid
      memset(valid.data(), 0, n * sizeof(uint8_t));

      // Run serial version
      KernelSerial();

      // Verify results match
      for (size_t i = 0; i < n; i++) {
        if (valid[i] != valid_parallel[i]) {
          printf("Kernel mismatch at %ld: serial=%d, parallel=%d\n", i,
                 valid[i], valid_parallel[i]);
          assert(false);
        }
      }
    }
  }

  void Run() {
    if (n == 0) {
      return;
    }

    valid = std::vector<uint8_t>(n, 0);
    
    DEBUG_ONLY {
      // L(i) < L(i+1) and R(i) < R(i+1)
      parlay::parallel_for(0, n - 1, [&](size_t i) {
        assert(L(i) < L(i + 1) && R(i) < R(i + 1));
      });

      // L(i) < R(i)
      parlay::parallel_for(0, n, [&](size_t i) {
        assert(L(i) < R(i));
      });

      // L(i + 1) <= R(i)
      parlay::parallel_for(0, n - 1, [&](size_t i) {
        assert(L(i + 1) <= R(i));
      });
    }

    Kernel();
  }

  static constexpr size_t parallel_block_size = parlay::internal::_block_size;
  // kNullPtr must fit in 62 bits (LinkListNode.nxt is 62 bits)
  static constexpr size_t kNullPtr = (1ULL << 62) - 1;

  size_t n;
  GetL L;
  GetR R;

  std::vector<LinkListNode> link_list;
  std::vector<uint8_t> valid;
  std::vector<size_t> furthest_id;
  std::vector<size_t> sampled_id, sampled_id_nxt;
};

#endif