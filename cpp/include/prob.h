#pragma once

#include <cstdint>
#include <ext/pb_ds/assoc_container.hpp>
#include <map>
#include <unordered_map>
#include <vector>

class MultiDist;
class State {
  static constexpr int MASK_BITS = 7;
  static constexpr std::uint64_t MASK = (1ULL << MASK_BITS) - 1;

  std::uint64_t val;
  int rem;

public:
  State(const MultiDist &md) : val{0}, rem{0} {}
  State(const MultiDist &md, const std::vector<int> &v);

  std::vector<int> to_vector() const;
  int operator[](int idx) const;
  void set_at(int idx, int x);
  int remaining() const;

  operator std::uint64_t() const { return val; }
};

struct chash {
  // any random-ish large odd number will do
  static constexpr uint64_t C = uint64_t(2e18 * M_PI) + 71;
  // random 32-bit number
  size_t operator()(std::uint64_t x) const {
    // see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
    return __builtin_bswap64(x * C);
  }
};
using ProbDist = __gnu_pbds::gp_hash_table<std::uint64_t, double, chash>;

class MultiDist {
public:
  const int n_arms, n_states;

  MultiDist(int n_arms, int n_states) : n_arms{n_arms}, n_states{n_states} {}

  void multinomialDistribution(int n, const std::vector<double> &probs,
                               ProbDist &out, State current, int idx = 0);

  ProbDist convolve(const ProbDist &a, const ProbDist &b);

  ProbDist
  fullTransitionDistribution(const std::vector<int> &S,
                             const std::vector<int> &A,
                             const std::vector<std::vector<double>> &p0,
                             const std::vector<std::vector<double>> &p1);
};
