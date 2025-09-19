#include "prob.h"

#include <array>
#include <cmath>
#include <numeric>

using namespace std;

constexpr int MAX_FACT_N = 100;
constexpr array<double, MAX_FACT_N + 1> factorials = [] {
  array<double, MAX_FACT_N + 1> arr;
  arr[0] = 1;
  for (int i = 1; i <= MAX_FACT_N; ++i) {
    arr[i] = arr[i - 1] * i;
  }
  return arr;
}();

void MultiDist::multinomialDistribution(int n, const vector<double> &probs,
                                        ProbDist &out, BitArray current,
                                        int idx) {
  if (idx == n_states - 1) {
    int _x = n - (current.sum() - current[n_states - 1]);
    current.set_at(idx, _x);

    double p = 1.0;
    double multinomialCoeff = factorials[n];
    for (int i = 0; i < n_states; ++i) {
      multinomialCoeff /= factorials[current[i]];
      p *= pow(probs[i], current[i]);
    }

    p *= multinomialCoeff;

    out[current] += p;
    return;
  }

  int remaining = n - current.sum();
  for (int i = 0; i <= remaining; ++i) {
    current.set_at(idx, i);
    multinomialDistribution(n, probs, out, current, idx + 1);
  }
}

ProbDist MultiDist::convolve(const ProbDist &a, const ProbDist &b) {
  ProbDist result;
  for (auto &[sa, pa] : a) {
    for (auto &[sb, pb] : b) {
      result[sa + sb] += pa * pb;
    }
  }
  return result;
}

ProbDist
MultiDist::fullTransitionDistribution(const BitArray &S, const vector<int> &A,
                                      const vector<vector<double>> &p0,
                                      const vector<vector<double>> &p1) {
  ProbDist total;
  BitArray initial;
  total[initial] = 1.0;
  for (int i = 0; i < n_states; ++i) {
    ProbDist dist1, dist0, combined;
    multinomialDistribution(A[i], p1[i], dist1, initial);
    multinomialDistribution(S[i] - A[i], p0[i], dist0, initial);
    combined = convolve(dist1, dist0);
    total = convolve(total, combined);
  }
  return total;
}
