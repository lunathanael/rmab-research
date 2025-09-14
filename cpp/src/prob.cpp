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

State::State(const MultiDist &md, const std::vector<int> &v)
    : val{0}, rem{0} {
  for (int i = 0; i < v.size(); ++i) {
    set_at(i, v[i]);
  }
}

std::vector<int> State::to_vector() const {
  std::vector<int> v;
  auto x = val;
  while(val) {
    v.emplace_back(x%MASK);
    x >>= MASK_BITS;
  }
  return v;
}

int State::operator[](int idx) const {
  return (val >> (idx * MASK_BITS)) & MASK;
}
void State::set_at(int idx, int x) {
  int diff = (x - this->operator[](idx));
  val = (val & ~(MASK << (idx * MASK_BITS))) | (x << (idx * MASK_BITS));
  rem += diff;
}
int State::remaining() const { return rem; }

void MultiDist::multinomialDistribution(int n, const vector<double> &probs,
                                        ProbDist &out, State current, int idx) {
  if (idx == n_states - 1) {
    int _x = n - (current.remaining() - current[n_states-1]);
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

  int remaining = n - current.remaining();
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

ProbDist MultiDist::fullTransitionDistribution(
    const vector<int> &S, const vector<int> &A,
    const vector<vector<double>> &p0, const vector<vector<double>> &p1) {
  ProbDist total;
  State initial(*this);
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
