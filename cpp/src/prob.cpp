#include "prob.h"

#include <cmath>
#include <numeric>

using namespace std;

State addStates(const State &a, const State &b) {
  State res(a.size());
  for (size_t i = 0; i < a.size(); ++i)
    res[i] = a[i] + b[i];
  return res;
}

void multinomialDistribution(int n, const vector<double> &probs, ProbDist &out,
                             State current, int idx) {
  int k = probs.size();
  if (current.empty())
    current.assign(k, 0);
  if (idx == k - 1) {
    current[idx] = n - accumulate(current.begin(), current.end() - 1, 0);

    double p = 1.0;
    long long multinomialCoeff = 1;
    long long nFact = 1;
    for (int i = 1; i <= n; ++i)
      nFact *= i;
    multinomialCoeff = nFact;
    for (int i = 0; i < k; ++i) {
      long long xiFact = 1;
      for (int j = 1; j <= current[i]; ++j)
        xiFact *= j;
      multinomialCoeff /= xiFact;

      p *= pow(probs[i], current[i]);
    }

    p *= multinomialCoeff;

    out[current] += p;
    return;
  }
  int remaining = n - accumulate(current.begin(), current.end(), 0);
  for (int i = 0; i <= remaining; ++i) {
    current[idx] = i;
    multinomialDistribution(n, probs, out, current, idx + 1);
  }
}

ProbDist convolve(const ProbDist &a, const ProbDist &b) {
  ProbDist result;
  for (auto &[sa, pa] : a) {
    for (auto &[sb, pb] : b) {
      State merged = addStates(sa, sb);
      result[merged] += pa * pb;
    }
  }
  return result;
}

ProbDist fullTransitionDistribution(const vector<int> &S, const vector<int> &A,
                                    const vector<vector<double>> &p0,
                                    const vector<vector<double>> &p1) {
  int k = S.size();
  ProbDist total;
  total[State(k, 0)] = 1.0;
  for (int i = 0; i < k; ++i) {
    ProbDist dist1, dist0, combined;
    multinomialDistribution(A[i], p1[i], dist1);
    multinomialDistribution(S[i] - A[i], p0[i], dist0);
    combined = convolve(dist1, dist0);
    total = convolve(total, combined);
  }
  return total;
}
