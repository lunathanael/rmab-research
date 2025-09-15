#pragma once

#include "utils.h"
#include <cstdint>
#include <ext/pb_ds/assoc_container.hpp>
#include <map>
#include <unordered_map>
#include <vector>

struct chash {
  // any random-ish large odd number will do
  static constexpr uint64_t C = uint64_t(2e18 * M_PI) + 71;
  // random 32-bit number
  size_t operator()(std::uint64_t x) const {
    // see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
    return __builtin_bswap64(x * C);
  }
};

using _underlying_table_t =
    __gnu_pbds::gp_hash_table<std::uint64_t, double, chash>;
class ProbDist : public _underlying_table_t {
public:
  using _underlying_table_t::operator[];
};

class MultiDist {
public:
  const int n_arms, n_states;

  MultiDist(int n_arms, int n_states) : n_arms{n_arms}, n_states{n_states} {}

  void multinomialDistribution(int n, const std::vector<double> &probs,
                               ProbDist &out, BitArray current, int idx = 0);

  ProbDist convolve(const ProbDist &a, const ProbDist &b);

  ProbDist
  fullTransitionDistribution(const BitArray &S, const std::vector<int> &A,
                             const std::vector<std::vector<double>> &p0,
                             const std::vector<std::vector<double>> &p1);
};
