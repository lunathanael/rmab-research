#include "dp.h"
#include "utils.h"

using namespace std;

DPStateIterator::DPStateIterator(int n_arms, int n_states)
    : n{combinations(n_arms + n_states - 1, n_states - 1)}, n_arms(n_arms),
      n_states(n_states) {
  init();
}

void DPStateIterator::init() {
  comp.assign(n_states, 0);
  comp.back() = n_arms;
  lastNonZero = n_states - 1;
  idx = 0;
}

const vector<int> &DPStateIterator::current() const { return comp; }

int DPStateIterator::next() {
  if (idx >= n)
    return n;

  int i = n_states - 2;
  while (i >= 0 && comp[i] == 0)
    --i;

  ++comp[i];

  --comp[n_states - 1];

  int sum_tail = 0;
  for (int j = i + 1; j < n_states - 1; ++j) {
    sum_tail += comp[j];
    comp[j] = 0;
  }
  comp[n_states - 1] += sum_tail;
  return idx++;
}

DPLayer::DPLayer(int n_states, int n_arms)
    : n_arms{n_arms}, n_states{n_states} {
  dim = combinations(n_arms + n_states - 1, n_states - 1);
}

double& DPLayer::operator[](const DPStateIterator& dpstate) {
  return dp[dpstate.idx];
}
