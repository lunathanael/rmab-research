#include "dp.h"
#include "utils.h"
#include <algorithm>
#include <numeric>

using namespace std;

DPStateIterator::DPStateIterator(int n_arms, int n_states)
    : n{combinations(n_arms + n_states - 1, n_states - 1)}, n_arms(n_arms),
      n_states(n_states) {
  init();
}

void DPStateIterator::init() {
  comp.assign(n_states, 0);
  comp.back() = n_arms;
  idx = 0;
}

const vector<int> &DPStateIterator::current() const { return comp; }

bool DPStateIterator::next() {
    if (++idx >= n) return false;

    int i_pivot = n_states - 1;
    while (i_pivot >= 0 && comp[i_pivot] == 0) {
        i_pivot--;
    }
    comp[i_pivot]--;
    comp[i_pivot - 1]++;
    int remainder = comp[i_pivot];
    comp[i_pivot] = 0;
    comp[n_states - 1] += remainder;
    return true;
}

int DPStateIterator::size() const { return n; }

StateActionIterator::StateActionIterator(const DPStateIterator &it, int n_alpha)
    : x(it.current()), n_states(it.size()), n_alpha(n_alpha), done(false) {
  y.assign(n_states, 0);
  sum_y = 0;
}

const vector<int> &StateActionIterator::current() const { return y; }

bool StateActionIterator::next() {
  if (done)
    return false;

  if (sum_y == 0 && !started) {
    started = true;
    if (n_alpha == 0)
      done = true;
    return true;
  }

  for (int i = n_states - 1; i >= 0; --i) {
    if (y[i] < min(x[i], n_alpha - (sum_y - y[i]))) {
      ++y[i];
      sum_y = accumulate(y.begin(), y.end(), 0);

      for (int j = i + 1; j < n_states; ++j)
        y[j] = 0;
      sum_y = accumulate(y.begin(), y.end(), 0);

      if (sum_y > n_alpha)
        continue;

      int rem = n_alpha - sum_y;
      for (int j = n_states - 1; j > i && rem > 0; --j) {
        int add = min(rem, x[j]);
        y[j] = add;
        rem -= add;
      }
      sum_y = accumulate(y.begin(), y.end(), 0);

      if (sum_y == n_alpha)
        return true;
      else
        continue;
    }
  }

  done = true;
  return false;
}

DPLayer::DPLayer(int n_arms, int n_states)
    : n_arms{n_arms}, n_states{n_states} {
  dim = combinations(n_arms + n_states - 1, n_states - 1);
  dp.assign(dim, 0);
}

double &DPLayer::operator[](const DPStateIterator &dpstate) {
  return dp[dpstate.idx];
}

void swap(DPLayer &a, DPLayer &b) noexcept { a.dp.swap(b.dp); }
