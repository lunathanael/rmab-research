#pragma once

#include "utils.h"
#include <cstdint>
#include <vector>
#include <ankerl/unordered_dense.h>

using ProbDist = ankerl::unordered_dense::map<std::uint64_t, double>;

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
